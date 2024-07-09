const [canvas, ui] = document.getElementsByClassName("canvas");
const ctx = ui.getContext("2d");
const menu = document.getElementById("menu");
const checkbox = document.getElementById("traversal-checkbox");

const [gl, programInfo] = initializeGL(canvas);
const handler = new IntersectionHandler();
const camera = new Camera();
let mesh = null;
let tree = null;

const TARGET_FRAMERATE = 120;

let stepUnits = 0.2;
let useParallel = false;
let lastRenderTime = 0;

let movement = {
    left: 0,
    up: 0,
    forward: 0,
    roll: 0,
};


function distance2D(p1, p2) {
    return Math.sqrt(Math.pow(p1[0] - p2[0], 2) + Math.pow(p1[1] - p2[1], 2));
}


function isViewerActive() {
    return document.pointerLockElement == ui;
}


function loadParallelLibVersion() {
    const parLibScript = document.createElement("script");
    parLibScript.src = "web/js/rmilib_par.js";
    document.body.appendChild(parLibScript);

    parLibScript.onload = () => {
        const headersScript = document.createElement("script");
        headersScript.src = "coi-serviceworker.min.js";
        document.body.appendChild(headersScript);

        headersScript.onload = () => {
            useParallel = true;
            Module.onRuntimeInitialized = main;
        }

        headersScript.onerror = () => {
            alert("Can't setup COOP/COEP");
            location.reload();
        }
    }

    parLibScript.onerror = () => {
        alert("Can't load parallel lib version");
        location.reload();
    }
}


function loadSeqLibVersion() {
    const libScript = document.createElement("script");
    libScript.src = "web/js/rmilib_seq.js";
    document.body.appendChild(libScript);

    libScript.onload = () => {
        useParallel = false;
        Module.onRuntimeInitialized = main;
    }

    libScript.onerror = () => {
        alert("Can't load seq lib version");
    }
}


// Remove dialog, lock or unlock slider and setup canvas
function main() {
    document.getElementById("dialog").remove();
    threadsCountSlider.disabled = !useParallel;
    menu.style.visibility = "visible";

    resizeCanvas();

    gl.clear(gl.COLOR_BUFFER_BIT);

    window.onresize = () => {
        resizeCanvas();
        draw();
    };
}


function updateTree() {
    if (!mesh) return;

    handler.clear();

    let splitter;
    switch (document.querySelector('input[name="splitter"]:checked').value) {
    case "sah":    splitter = Module.Splitter.SAH;    break;
    case "median": splitter = Module.Splitter.Median; break;
    }

    tree = new Tree(Module.KDTree.forMesh(mesh.data, splitter));
}


function takeScreenShot() {
    const currentDate = new Date();
    const link = document.createElement('a');
    draw();
    link.download = `rmi_screenshot_${currentDate.toISOString()}.png`;
    link.href = canvas.toDataURL({
        format: "png",
    });
    link.click();
}


ui.onclick = () => {
    if (mesh && !isViewerActive()) {
        ui.requestPointerLock();
    } else if (isViewerActive()) {
        const ray = camera.eyeRay();
        if (checkbox.checked) {
            handler.startTraversal(ray, tree);
        } else {
            handler.findIntersections(ray, tree);
        }
    }
}


document.onpointerlockchange = (event) => {
    if (isViewerActive()) {
        menu.style.visibility = "hidden";
        window.requestAnimationFrame(update);
    } else {
        menu.style.visibility = "visible";

        movement.up = 0;
        movement.forward = 0;
        movement.left = 0;
        movement.roll = 0;
    }
};


ui.onwheel = (event) => {
    if (!isViewerActive()) return;

    if (event.deltaY < 0) {
        stepUnits = Math.min(stepUnits + 1, 20);
    } else {
        stepUnits = Math.max(stepUnits - 3, 0.2);
    }
    console.log(stepUnits)
}


ui.onmousemove = (event) => {
    if (isViewerActive()) {
        camera.rotate(-event.movementX, event.movementY, 0);
    }
};


onkeydown = (event) => {
    if (!isViewerActive()) {
        return;
    }

    switch (event.code) {
        case "KeyQ":      movement.roll = -1.0;           break;
        case "KeyE":      movement.roll = +1.0;           break;
        case "KeyW":      movement.forward = +stepUnits; break;
        case "KeyS":      movement.forward = -stepUnits; break;
        case "KeyA":      movement.left    = +stepUnits; break;
        case "KeyD":      movement.left    = -stepUnits; break;
        case "Space":     movement.up      = +stepUnits; break;
        case "ShiftLeft": movement.up      = -stepUnits; break;
        case "ArrowUp":   handler.traverse();             break;
        case "Enter":     takeScreenShot();               break;
    }
};


onkeyup = (event) => {
    const key = event.code;
    if ((key == "KeyW" && movement.forward > 0) || (key == "KeyS" && movement.forward < 0)) {
        movement.forward = 0;
    } else if ((key == "KeyA" && movement.left > 0) || (key == "KeyD" && movement.left < 0)) {
        movement.left = 0;
    } else if ((key == "Space" && movement.up > 0) || (key == "ShiftLeft" && movement.up < 0)) {
        movement.up = 0;
    } else if ((key == "KeyE" && movement.roll > 0) || (key == "KeyQ" && movement.roll < 0)) {
        movement.roll = 0;
    }
};


// Read file, pass it's content to wasm, turn on loading animation
document.getElementById("file-upload").onchange = function() {
    const file = this.files[0];
    if (!file) {
        return;
    }

    const label = document.querySelector('label[for="file-upload"]');
    label.classList.toggle("button-loading");

    const reader = new FileReader();
    reader.readAsArrayBuffer(file);

    handler.clear();

    reader.onload = () => {
        try {
            mesh = new Mesh(Module.readMesh(file.name, reader.result));
            updateTree();
            draw();
        } catch(err) {
            alert("Unsupported mesh format");
            console.error(err);
        } finally {
            label.classList.toggle("button-loading");
        }
    };
}


// Load shaders, setup gl, create global texture
function initializeGL(canvas) {
    let gl = canvas.getContext("webgl2") || canvas.getContext("experimental-webgl");
    if (!gl) {
        alert("Your browser doesn't support WebGL");
        return;
    }
    let programInfo = twgl.createProgramInfo(gl, [
        document.getElementById("shader-vs").firstChild.textContent,
        document.getElementById("shader-fs").firstChild.textContent,
    ]);
    twgl.setAttributePrefix("a_");
    gl.useProgram(programInfo.program);

    gl.clearColor(0.23, 0.25, 0.27, 1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

    // creating texture with visible outline and transparent body
    const texCanvas = document.createElement("canvas")
    texCanvas.width = 256
    texCanvas.height = 256
    const ctx = texCanvas.getContext("2d");
    ctx.fillStyle = "white"
    ctx.fillRect(0, 0, texCanvas.width, texCanvas.height)
    ctx.clearRect(1, 1, 254, 254);

    twgl.createTexture(gl, {
        width: texCanvas.width, height: texCanvas.height,
        format: gl.RGBA, internalFormat: gl.RGBA, type: gl.UNSIGNED_BYTE,
        min: gl.NEAREST, mag: gl.NEAREST,
        wrapS: gl.CLAMP_TO_EDGE, wrapT: gl.CLAMP_TO_EDGE,
        src: ctx.getImageData(0, 0, texCanvas.width, texCanvas.height).data
    });

    return [gl, programInfo]
}


function update(curTime) {
    if (curTime - lastRenderTime >= 1000 / TARGET_FRAMERATE) {
        camera.move(movement.forward, movement.left, movement.up);
        if (movement.roll) {
            camera.rotate(0, 0, movement.roll);
        }
        draw();
    }
    lastRenderTime = curTime;
    if (isViewerActive()) {
        window.requestAnimationFrame(update);
    }
}


function drawPointLabel(
    point,
    persist = false,
    viewDistance = 100,
    maxFontSize = 256
) {
    const pos = camera.projectToScreen(point);
    if (pos[2] < 0 || (!persist && distance2D(pos, [ui.width / 2, ui.height / 2]) > viewDistance)) {
        return;
    }

    ctx.fillStyle = "#fed053";
    const fontSize = maxFontSize / Math.min(1.0 + pos[2], 10);
    ctx.font = `${fontSize}px Arial`;
    ctx.fillText(`${toFixed(point[0])}, ${toFixed(point[1])}, ${toFixed(point[2])}`, pos[0], pos[1]);
}


function drawUI() {
    ctx.clearRect(0, 0, ui.width, ui.height);

    if (handler.ray) {
        drawPointLabel(handler.ray.at(0), true);

        for (let i = 0; i < handler.coords.length; i += 3) {
            drawPointLabel([handler.coords[i], handler.coords[i+1], handler.coords[i+2]]);
        }

        ctx.fillStyle = "#fed053";
        ctx.font = "24px Arial";
        ctx.fillText(handler.info, 50, 50);
    }

    // update center point
    ctx.beginPath();
        ctx.fillStyle = "#fed053";
        ctx.arc(ui.width / 2, ui.height / 2, 3, 0, 2 * Math.PI);
    ctx.fill();
}


function draw() {
    drawUI();

    gl.clear(gl.COLOR_BUFFER_BIT);
    camera.prepareScene();

    if (mesh) {
        mesh.draw();
        handler.draw();
    }
}


function resizeCanvas() {
    twgl.resizeCanvasToDisplaySize(canvas)
    twgl.resizeCanvasToDisplaySize(ui);

    gl.viewport(0, 0, canvas.width, canvas.height);
    camera.resize(canvas.width, canvas.height);
}

