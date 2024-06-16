class Drawable {
    constructor(color, type) {
        this.bufferInfo = null;
        this.vao = null;
        this.type = type;
        this.color = color;
    }

    update(arrays) {
        this.bufferInfo = twgl.createBufferInfoFromArrays(gl, arrays);
        this.vao = twgl.createVAOFromBufferInfo(gl, programInfo, this.bufferInfo);
    }

    clear() {
        this.vao = null;
        this.bufferInfo = null;
    }

    draw() {
        if (this.vao && this.bufferInfo) {
            twgl.setUniforms(programInfo, {
                u_colorMult: this.color
            });

            gl.bindVertexArray(this.vao);
            twgl.drawBufferInfo(gl, this.bufferInfo, this.type);
        }
    }
}

