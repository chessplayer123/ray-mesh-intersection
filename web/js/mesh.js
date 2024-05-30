class Mesh {
    constructor(gl, treeType) {
        this.mesh = null;
        this.vao = null;
        this.bufferInfo = null;
        this.treeType = treeType;
        this.tree = null;

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
            u_texture: this.texture,
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

