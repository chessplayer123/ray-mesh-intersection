const canvas = document.getElementById("canvas");
const mesh = new Mesh();
const camera = new Camera();
let gl;
let programInfo;
let moveLeft = 0;
let moveUp = 0;
let moveForward = 0;


window.onresize = () => {
    resizeCanvas();
    paint();
};


Module.onRuntimeInitialized = () => {
    initializeGL();
    paint();
};


canvas.onclick = canvas.requestPointerLock;


canvas.onmousemove = (event) => {
    if (document.pointerLockElement == canvas) {
        camera.rotate(event.movementX, -event.movementY);
        paint();
    }
};


onkeypress = (event) => {
    switch (event.key) {
        case "w": moveForward = 1; break;
        case "s": moveForward = -1; break;
        case "a": moveLeft = 1; break;
        case "d": moveLeft = -1; break;
        default: return;
    }
    camera.move(moveForward, moveLeft, moveUp);
    paint();
};


onkeyup = (event) => {
    switch (event.key) {
        case "w":
        case "s":
            moveForward = 0;
            break;
        case "d":
        case "a":
            moveLeft = 0;
            break;
        default:
            return;
    }
};


document.getElementById("file-upload").onchange = function() {
    const reader = new FileReader();
    const file = this.files[0];
    reader.readAsArrayBuffer(file);
    reader.onload = () => {
        mesh.update(gl, file.name, reader.result);
        paint();
    };
}


function initializeGL() {
    gl = canvas.getContext("webgl2") || canvas.getContext("experimental-webgl");
    if (!gl) {
        alert("Your browser doesn't support WebGL");
    }
    programInfo = twgl.createProgramInfo(gl, [
        document.getElementById("shader-vs").firstChild.textContent,
        document.getElementById("shader-fs").firstChild.textContent,
    ]);
    twgl.setAttributePrefix("a_");
    gl.useProgram(programInfo.program);

    gl.clearColor(0.23, 0.25, 0.27, 1.0);
    gl.enable(gl.CULL_FACE);
    gl.enable(gl.DEPTH_TEST);

    resizeCanvas();
}


function paint() {
    gl.clear(gl.COLOR_BUFFER_BIT);
    camera.prepareScene();
    if (mesh.isLoaded()) {
        mesh.draw(gl);
    }
}


function resizeCanvas() {
    canvas.width = canvas.clientWidth;
    canvas.height = canvas.clientHeight * 0.95;
    gl.viewport(0, 0, canvas.width, canvas.height);
    camera.resize(canvas.width, canvas.height);
}

