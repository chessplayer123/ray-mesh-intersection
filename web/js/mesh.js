class Mesh {
    constructor(gl, treeType) {
        this.mesh = null;
        this.vao = null;
        this.bufferInfo = null;
        this.treeType = treeType;
        this.tree = null;
        this.color = [1, 1, 1, 1];
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
        this.buildTree();
    }

    buildTree() {
        switch (this.treeType) {
            case 1: this.tree = Module.KDTree.forMesh(this.mesh); break;
            case 2: this.tree = Module.Quadtree.forMesh(this.mesh); break;
            case 3: this.tree = Module.Octree.forMesh(this.mesh); break;
        }
    }

    setTreeType(type) {
        this.treeType = type;
        if (this.isLoaded()) {
            this.buildTree();
        }
    }

    isLoaded() {
        return this.mesh != null;
    }

    draw(gl) {
        twgl.setUniforms(programInfo, {
            u_colorMult: this.color,
        });

        gl.bindVertexArray(this.vao);
        twgl.drawBufferInfo(gl, this.bufferInfo, gl.TRIANGLES);
    }

    intersects(ray) {
        switch (this.treeType) {
            case 1: return ray.intersects_kdtree(mesh.tree);
            case 2: return ray.intersects_quadtree(mesh.tree);
            case 3: return ray.intersects_octree(mesh.tree);
        }
    }

    par_intersects(ray, threadsCount) {
        switch (this.treeType) {
            case 1: return ray.par_intersects_kdtree(mesh.tree, threadsCount);
            case 2: return ray.par_intersects_quadtree(mesh.tree, threadsCount);
            case 3: return ray.par_intersects_octree(mesh.tree, threadsCount);
        }
    }
}

