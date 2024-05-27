const canvas = document.getElementById("canvas");
const infoField = document.getElementById("help-message");
const intersectionsData = document.getElementById("app-data");
const mesh = new Mesh();
const camera = new Camera();
const points = new Points();
const maxFramerate = 120;
const stepUnits = 0.2;
let lastRenderTime = 0;

let moveLeft = 0;
let moveUp = 0;
let moveForward = 0;

let gl;
let programInfo;
let texture;


function toFixed(num, digitsCount=2) {
    return Number(num).toFixed(digitsCount);
}


window.onresize = () => {
    resizeCanvas();
    paint();
};


Module.onRuntimeInitialized = () => {
    initializeGL();
    resizeCanvas();
    gl.clear(gl.COLOR_BUFFER_BIT);
};


canvas.onclick = () => {
    if (mesh.isLoaded()) {
        canvas.requestPointerLock();
    }
}


document.onpointerlockchange = (event) => {
    if (document.pointerLockElement == canvas) {
        infoField.style.visibility = "hidden";
        window.requestAnimationFrame(update);
    } else {
        infoField.style.visibility = "visible";

        moveForward = 0;
        moveUp = 0;
        moveLeft = 0;
    }
};


canvas.onmousemove = (event) => {
    if (document.pointerLockElement == canvas) {
        camera.rotate(event.movementX, -event.movementY);
    }
};


onkeydown = (event) => {
    if (document.pointerLockElement != canvas) {
        return;
    }

    switch (event.code) {
        case "KeyQ":      findIntersections();      break;
        case "KeyW":      moveForward =  stepUnits; break;
        case "KeyS":      moveForward = -stepUnits; break;
        case "KeyA":      moveLeft    =  stepUnits; break;
        case "KeyD":      moveLeft    = -stepUnits; break;
        case "Space":     moveUp      =  stepUnits; break;
        case "ShiftLeft": moveUp      = -stepUnits; break;
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


document.getElementById("file-upload").onchange = function() {
    const reader = new FileReader();
    const file = this.files[0];
    reader.readAsArrayBuffer(file);

    intersectionsData.style.visibility = "hidden";
    points.clear();

    reader.onload = () => {
        mesh.update(gl, programInfo, file.name, reader.result);
        paint();
    };
}


function findIntersections() {
    const ray = camera.eyeRay();

    const start = performance.now();
    const intersections = ray.intersects_tree(mesh.kdtree);
    const timeSpent = toFixed(performance.now() - start, 3);

    let data = [
        `Position(${toFixed(camera.pos[0])}, ${toFixed(camera.pos[1])}, ${toFixed(camera.pos[2])})`,
        `Direction(${toFixed(camera.front[0])}, ${toFixed(camera.front[1])}, ${toFixed(camera.front[2])})`,
        `Found: ${intersections.size()} (${timeSpent} ms)`,
    ];
    data.push(...points.update(gl, programInfo, intersections))

    intersectionsData.innerHTML = data.join("\n");
    intersectionsData.style.visibility = "visible";
}


function initializeGL() {
    gl = canvas.getContext("webgl2") || canvas.getContext("experimental-webgl");
    if (!gl) {
        alert("Your browser doesn't support WebGL");
        return;
    }
    programInfo = twgl.createProgramInfo(gl, [
        document.getElementById("shader-vs").firstChild.textContent,
        document.getElementById("shader-fs").firstChild.textContent,
    ]);
    twgl.setAttributePrefix("a_");
    gl.useProgram(programInfo.program);

    gl.clearColor(0.23, 0.25, 0.27, 1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

    const texCanvas = document.createElement("canvas")
    texCanvas.width = 256
    texCanvas.height = 256
    const ctx = texCanvas.getContext("2d");
    ctx.fillStyle = "rgba(255, 255, 255, 255)"
    ctx.fillRect(0, 0, texCanvas.width, texCanvas.height)
    ctx.clearRect(1, 1, 254, 254);

    texture = twgl.createTexture(gl, {
        width: texCanvas.width, height: texCanvas.height,
        format: gl.RGBA, internalFormat: gl.RGBA, type: gl.UNSIGNED_BYTE,
        min: gl.NEAREST, mag: gl.NEAREST,
        wrapS: gl.CLAMP_TO_EDGE, wrapT: gl.CLAMP_TO_EDGE,
        src: ctx.getImageData(0, 0, texCanvas.width, texCanvas.height).data
    })
}


function update(curTime) {
    if (curTime - lastRenderTime >= 1000 / maxFramerate) {
        camera.move(moveForward, moveLeft, moveUp);
        paint();
    }
    lastRenderTime = curTime;
    if (document.pointerLockElement == canvas) {
        window.requestAnimationFrame(update);
    }
}


function paint() {
    gl.clear(gl.COLOR_BUFFER_BIT);
    camera.prepareScene();
    if (mesh.isLoaded()) {
        mesh.draw(gl);

        if (!points.isEmpty()) {
            points.draw(gl);
        }
    }
}


function resizeCanvas() {
    twgl.resizeCanvasToDisplaySize(canvas)
    gl.viewport(0, 0, canvas.width, canvas.height);
    camera.resize(canvas.width, canvas.height);
}

