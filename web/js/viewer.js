const canvas = document.getElementById("canvas");
const mesh = new Mesh();
const camera = new Camera();
const maxFramerate = 120;
const stepUnits = 0.2
let lastRenderTime = 0;

let moveLeft = 0;
let moveUp = 0;
let moveForward = 0;

let gl;
let programInfo;

let pointsVAO;
let pointsBufferInfo;
let texture;


window.onresize = () => {
    resizeCanvas;
    paint();
}


Module.onRuntimeInitialized = () => {
    initializeGL();
    resizeCanvas();
};


canvas.onclick = async () => {
    if (mesh.isLoaded()) {
        await canvas.requestPointerLock();
        window.requestAnimationFrame(update);
    }
}


canvas.onmousemove = (event) => {
    if (document.pointerLockElement == canvas) {
        camera.rotate(event.movementX, -event.movementY);
    }
};


onkeydown = (event) => {
    if (document.pointerLockElement != canvas) {
        return;
    }

    console.log(event)
    switch (event.key) {
        case "q":
            const ray = camera.eyeRay();
            const points = ray.intersects_tree(mesh.kdtree);
            let coords = [];
            for (let i = 0; i < points.size(); ++i) {
                const p = points.get(i);
                coords.push(p.x, p.y, p.z);
            }
            let v = new Float32Array(coords)
            pointsBufferInfo = twgl.createBufferInfoFromArrays(gl, {
                position: v
            })
            pointsVAO = twgl.createVAOFromBufferInfo(gl, programInfo, pointsBufferInfo);
            break;
        case "w":     moveForward =  stepUnits; break;
        case "s":     moveForward = -stepUnits; break;
        case "a":     moveLeft    =  stepUnits; break;
        case "d":     moveLeft    = -stepUnits; break;
        case " ":     moveUp      =  stepUnits; break;
        case "Shift": moveUp      = -stepUnits; break;
    }
};


onkeyup = (event) => {
    if (event.key == "w" && moveForward > 0) {
        moveForward = 0;
    } else if (event.key == "s" && moveForward < 0) {
        moveForward = 0;
    } else if (event.key == "d" && moveLeft < 0) {
        moveLeft = 0;
    } else if (event.key == "a" && moveLeft > 0) {
        moveLeft = 0;
    } else if (event.key == " " && moveUp > 0) {
        moveUp = 0;
    } else if (event.key == "Shift" && moveUp < 0) {
        moveUp = 0;
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

        if (pointsVAO) {
            gl.bindVertexArray(pointsVAO);
            twgl.drawBufferInfo(gl, pointsBufferInfo, gl.POINTS);
        }
    }
}


function resizeCanvas() {
    canvas.width = canvas.clientWidth;
    canvas.height = canvas.clientHeight * 0.95;
    gl.viewport(0, 0, canvas.width, canvas.height);
    camera.resize(canvas.width, canvas.height);
}

