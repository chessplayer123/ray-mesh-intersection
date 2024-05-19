const canvas = document.getElementById("canvas");
const gl = canvas.getContext("webgl");
const input = document.getElementById("input");
let mesh = null;

window.onload = resizeCanvas();
window.onresize = resizeCanvas();
input.addEventListener("change", readMesh, false);


// webgl functions
function initializeGL() {
    gl.clearColor(0, 0, 0, 1);
    gl.enable(gl.DEPTH_TEST);
}


function resizeGL() {
    gl.viewport(0, 0, canvas.width, canvas.height);
    paintGL();
}


function paintGL() {
    gl.clear(gl.COLOR_BUFFER_BIT);

    if (mesh) {
        drawMesh();
    }
}

function drawMesh() {
}
//


Module.onRuntimeInitialized = () => {
    initializeGL();
    paintGL();
}


function resizeCanvas() {
    canvas.width = canvas.clientWidth
    canvas.height = canvas.clientHeight + "px"
    resizeGL();
}


async function readMesh() {
    const files = this.files;
    const text = await files[0].text()
    const mesh = await Module.readMesh(files[0].name, text);
    paintGL();
}
