const [canvas, ui] = document.getElementsByClassName("canvas");
const uiContext = ui.getContext("2d");
const menu = document.getElementById("menu");
const checkbox = document.getElementById("traversal-checkbox");

const [gl, programInfo] = initializeGL(canvas);
const mesh = new Mesh(gl);
const handler = new IntersectionHandler();
const tree = new Tree(parseInt(document.querySelector('input[name="tree"]:checked').value));
const camera = new Camera();

const TARGET_FRAMERATE = 120;
const STEP_UNITS = 0.2;

let useParallel = false;
let lastRenderTime = 0;
let moveLeft = 0;
let moveUp = 0;
let moveForward = 0;


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
            Module.onRuntimeInitialized = start;
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
        Module.onRuntimeInitialized = start;
    }

    libScript.onerror = () => {
        alert("Can't load seq lib version");
    }
}


// Remove dialog, lock or unlock slider and setup canvas
function start() {
    document.getElementById("dialog").remove();
    threadsCountSlider.disabled = !useParallel;
    menu.style.visibility = "visible";

    resizeCanvas();

    gl.clear(gl.COLOR_BUFFER_BIT);

    window.onresize = () => {
        resizeCanvas();
        paint();
    };
}


function onRadioChanged(option) {
    if (mesh.isLoaded()) {
        tree.build(mesh, parseInt(option.value));
        handler.clear();
    }
}


ui.onclick = () => {
    if (mesh.isLoaded() && !isViewerActive()) {
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

        moveForward = 0;
        moveUp = 0;
        moveLeft = 0;
    }
};


ui.onmousemove = (event) => {
    if (isViewerActive()) {
        camera.rotate(event.movementX, -event.movementY);
    }
};


onkeydown = (event) => {
    if (!isViewerActive()) {
        return;
    }

    switch (event.code) {
        case "ArrowUp":   handler.traverse();        break;
        case "KeyW":      moveForward =  STEP_UNITS; break;
        case "KeyS":      moveForward = -STEP_UNITS; break;
        case "KeyA":      moveLeft    =  STEP_UNITS; break;
        case "KeyD":      moveLeft    = -STEP_UNITS; break;
        case "Space":     moveUp      =  STEP_UNITS; break;
        case "ShiftLeft": moveUp      = -STEP_UNITS; break;
    }
};


onkeyup = (event) => {
    const key = event.code;
    if (key == "KeyW" && moveForward > 0) {
        moveForward = 0;
    } else if (key == "KeyS" && moveForward < 0) {
        moveForward = 0;
    } else if (key == "KeyD" && moveLeft < 0) {
        moveLeft = 0;
    } else if (key == "KeyA" && moveLeft > 0) {
        moveLeft = 0;
    } else if (key == "Space" && moveUp > 0) {
        moveUp = 0;
    } else if (key == "ShiftLeft" && moveUp < 0) {
        moveUp = 0;
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
            mesh.update(gl, programInfo, file.name, reader.result);
            tree.build(mesh, parseInt(document.querySelector('input[name="tree"]:checked').value));
            paint();
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
        camera.move(moveForward, moveLeft, moveUp);
        paint();
    }
    lastRenderTime = curTime;
    if (isViewerActive()) {
        window.requestAnimationFrame(update);
    }
}


function paint() {
    gl.clear(gl.COLOR_BUFFER_BIT);
    camera.prepareScene();
    if (mesh.isLoaded()) {
        mesh.draw(gl);
        handler.draw();
    }
}


function resizeCanvas() {
    twgl.resizeCanvasToDisplaySize(canvas)
    gl.viewport(0, 0, canvas.width, canvas.height);
    camera.resize(canvas.width, canvas.height);

    // update center point
    twgl.resizeCanvasToDisplaySize(ui);
    uiContext.beginPath();
        uiContext.fillStyle = "#fed053";
        uiContext.arc(ui.width / 2, ui.height / 2, 3, 0, 2 * Math.PI);
    uiContext.fill();
}

