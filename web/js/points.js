
class Points {
    constructor() {
        this.vao = null;
        this.bufferInfo = null;
    }

    draw(gl) {
        twgl.setUniforms(programInfo, {
            u_colorMult: [0.96, 0.3, 0.0, 1]
        });

        gl.bindVertexArray(this.vao);
        twgl.drawBufferInfo(gl, this.bufferInfo, gl.POINTS);
    }

    isEmpty() {
        return this.vao == null;
    }

    update(gl, programInfo, points) {
        if (points.size() == 0) {
            this.vao = null;
            this.bufferInfo = null;
            return [];
        }

        let coords = [];
        let pointsRepr = [];
        for (let i = 0; i < points.size(); ++i) {
            const p = points.get(i);
            coords.push(p.x, p.y, p.z);
            pointsRepr.push(`  (${toFixed(p.x)}, ${toFixed(p.y)}, ${toFixed(p.z)})`)
        }
        this.bufferInfo = twgl.createBufferInfoFromArrays(gl, {
            position: new Float32Array(coords)
        })
        this.vao = twgl.createVAOFromBufferInfo(gl, programInfo, this.bufferInfo);

        return pointsRepr;
    }

    clear() {
        this.vao = null;
        this.bufferInfo = null;
    }
};
