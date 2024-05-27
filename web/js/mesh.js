class Mesh {
    constructor(gl) {
        this.mesh = null;
        this.vao = null;
        this.bufferInfo = null;
        this.kdtree = null;

        const texCanvas = document.createElement("canvas")
        texCanvas.width = 256
        texCanvas.height = 256
        const ctx = texCanvas.getContext("2d");
        ctx.fillStyle = "rgba(255, 255, 255, 255)"
        ctx.fillRect(0, 0, texCanvas.width, texCanvas.height)
        ctx.clearRect(1, 1, 254, 254);

        this.texture = twgl.createTexture(gl, {
            width: texCanvas.width, height: texCanvas.height,
            format: gl.RGBA, internalFormat: gl.RGBA, type: gl.UNSIGNED_BYTE,
            min: gl.NEAREST, mag: gl.NEAREST,
            wrapS: gl.CLAMP_TO_EDGE, wrapT: gl.CLAMP_TO_EDGE,
            src: ctx.getImageData(0, 0, texCanvas.width, texCanvas.height).data
        })

    }

    update(gl, programInfo, filename, data) {
        this.mesh = Module.readMesh(filename, data);

        let texCoords = [];
        for (let i = 0; i < this.mesh.size; ++i) {
            texCoords.push(0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0)
        }

        this.bufferInfo = twgl.createBufferInfoFromArrays(gl, {
            position: this.mesh.vertices(),
            texcoord: new Float32Array(texCoords),
            indices: this.mesh.indices(),
        })
        this.vao = twgl.createVAOFromBufferInfo(gl, programInfo, this.bufferInfo)
        this.kdtree = Module.KDTree.forMesh(this.mesh);
    }

    isLoaded() {
        return this.mesh != null;
    }

    draw(gl) {
        twgl.setUniforms(programInfo, {
            u_texture: this.texture,
        });

        gl.bindVertexArray(this.vao);
        twgl.drawBufferInfo(gl, this.bufferInfo, gl.TRIANGLES);
    }
}

