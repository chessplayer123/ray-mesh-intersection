class Mesh {
    constructor() {
        this.mesh = null;
        this.vao = null;
        this.bufferInfo = null;
        this.kdtree = null;
    }

    update(gl, filename, data) {
        this.mesh = Module.readMesh(filename, data);

        this.bufferInfo = twgl.createBufferInfoFromArrays(gl, {
            position: this.mesh.vertices(),
            indices: this.mesh.indices(),
        })
        this.vao = twgl.createVAOFromBufferInfo(gl, programInfo, this.bufferInfo)
        gl.bindVertexArray(mesh.vao);
        this.kdtree = Module.KDTree.forMesh(this.mesh);
    }

    isLoaded() {
        return this.mesh != null;
    }

    draw(gl) {
        twgl.drawBufferInfo(gl, mesh.bufferInfo, gl.LINES);
    }
}

