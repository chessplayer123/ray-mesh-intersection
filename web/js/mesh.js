class Mesh {
    constructor(data) {
        this.data = data;

        const texCoords = [];
        for (let i = 0; i < this.data .size; ++i) {
            texCoords.push(0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0)
        }

        this.drawable = new Drawable({
            position: this.data.vertices(),
            texcoord: texCoords,
            indices: this.data.indices(),
        }, [1, 1, 1, 1], gl.TRIANGLES_STRIP);
    }

    draw(gl) {
        this.drawable.draw();
    }
}

