class Drawable {
    constructor(arrays, color, type) {
        this.type = type;
        this.color = color;

        this.bufferInfo = twgl.createBufferInfoFromArrays(gl, arrays);
        this.vao = twgl.createVAOFromBufferInfo(gl, programInfo, this.bufferInfo);
    }

    draw() {
        twgl.setUniforms(programInfo, {
            u_colorMult: this.color
        });

        gl.bindVertexArray(this.vao);
        twgl.drawBufferInfo(gl, this.bufferInfo, this.type);
    }
}

