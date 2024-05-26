class Mesh {
    constructor() {
        this.mesh = null;
        this.vao = null;
        this.bufferInfo = null;
        this.kdtree = null;
    }

    update(gl, filename, data) {
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
            u_texture: texture,
        });

        gl.bindVertexArray(this.vao);
        twgl.drawBufferInfo(gl, this.bufferInfo, gl.TRIANGLES);
    }
}

