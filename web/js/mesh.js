class Mesh {
    constructor(gl) {
        this.data = null;
        this.vao = null;
        this.bufferInfo = null;
        this.color = [1, 1, 1, 1];
        this.boxes = [];
    }

    update(gl, programInfo, filename, data) {
        this.data = Module.readMesh(filename, data);

        let texCoords = [];
        for (let i = 0; i < this.data .size; ++i) {
            texCoords.push(0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0)
        }

        this.bufferInfo = twgl.createBufferInfoFromArrays(gl, {
            position: this.data.vertices(),
            texcoord: texCoords,
            indices: this.data.indices(),
        })
        this.vao = twgl.createVAOFromBufferInfo(gl, programInfo, this.bufferInfo)
    }

    isLoaded() {
        return this.data != null;
    }

    draw(gl) {
        twgl.setUniforms(programInfo, {
            u_colorMult: this.color,
        });

        gl.bindVertexArray(this.vao);
        twgl.drawBufferInfo(gl, this.bufferInfo, gl.TRIANGLES_STRIP);
    }
}

