class Mesh {
    constructor(gl) {
        this.data = null;
        this.drawable = new Drawable([1, 1, 1, 1], gl.TRIANGLES_STRIP);
    }

    update(gl, programInfo, filename, data) {
        this.data = Module.readMesh(filename, data);

        let texCoords = [];
        for (let i = 0; i < this.data .size; ++i) {
            texCoords.push(0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0)
        }
        this.drawable.update({
            position: this.data.vertices(),
            texcoord: texCoords,
            indices: this.data.indices(),
        });
    }

    isLoaded() {
        return this.data != null;
    }

    draw(gl) {
        this.drawable.draw();
    }
}

