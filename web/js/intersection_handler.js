const intersectionsData = document.getElementById("app-data");
const threadsCountSlider = document.getElementById("threads-count");


function toFixed(num, digitsCount=2) {
    return Number(num).toFixed(digitsCount);
}


class IntersectionHandler {
    constructor() {
        this.clear();
    }

    extendPoints(points) {
        if (points.size() == 0) {
            return;
        }

        let pointsRepr = [];
        for (let i = 0; i < points.size(); ++i) {
            const p = points.get(i);
            this.coords.push(p.x, p.y, p.z);
            pointsRepr.push(`\n  (${toFixed(p.x)}, ${toFixed(p.y)}, ${toFixed(p.z)})`)
        }

        this.pointsBufferInfo = twgl.createBufferInfoFromArrays(gl, {
            position: this.coords
        })
        this.pointsVAO = twgl.createVAOFromBufferInfo(gl, programInfo, this.pointsBufferInfo);

        intersectionsData.innerHTML += pointsRepr.join();
    }

    findIntersections(ray, tree) {
        this.clear();

        let start = performance.now();
        let intersections = tree.intersects(ray);
        const seqTimeInfo = `${toFixed(performance.now() - start, 3)} ms`;

        let parTimeInfo = "-";
        if (useParallel) {
            start = performance.now();
            tree.par_intersects(ray, parseInt(threadsCountSlider.value));
            parTimeInfo = `${toFixed(performance.now() - start, 3)} ms`;
        }

        let data = [
            `Position  (${toFixed(camera.pos[0])}, ${toFixed(camera.pos[1])}, ${toFixed(camera.pos[2])})`,
            `Direction (${toFixed(camera.front[0])}, ${toFixed(camera.front[1])}, ${toFixed(camera.front[2])})`,
            `seq: ${seqTimeInfo}, par: ${parTimeInfo}`,
        ];

        intersectionsData.innerHTML = data.join("\n");
        intersectionsData.style.visibility = "visible";

        this.extendPoints(intersections);
    }

    startTraversal(ray, tree) {
        this.clear();
        tree.ascendToRoot();

        this.tree = tree;
        this.ray = ray;

        [this.boxesBufferInfo, this.boxesVAO] = this.tree.createBufferInfoAndVao();

        let data = [
            `Position  (${toFixed(camera.pos[0])}, ${toFixed(camera.pos[1])}, ${toFixed(camera.pos[2])})`,
            `Direction (${toFixed(camera.front[0])}, ${toFixed(camera.front[1])}, ${toFixed(camera.front[2])})`
        ];

        intersectionsData.innerHTML = data.join("\n");
        intersectionsData.style.visibility = "visible";
    }

    traverse() {
        if (this.ray && this.tree) {
            const intersections = this.tree.descendAlongRay(this.ray);
            this.extendPoints(intersections);
            [this.boxesBufferInfo, this.boxesVAO] = this.tree.createBufferInfoAndVao();
        }
    }

    clear() {
        intersectionsData.style.visibility = "hidden";

        this.ray = null;
        this.tree = null;

        this.coords = [];
        this.pointsVAO = null;
        this.pointsBufferInfo = null;

        this.boxesVAO = null;
        this.boxesBufferInfo = null;
    }

    draw() {
        if (this.pointsVAO) {
            twgl.setUniforms(programInfo, {
                u_colorMult: [0.96, 0.3, 0.0, 1]
            });

            gl.bindVertexArray(this.pointsVAO);
            twgl.drawBufferInfo(gl, this.pointsBufferInfo, gl.POINTS);
        }

        if (this.boxesVAO) {
            twgl.setUniforms(programInfo, {
                u_colorMult: [0.5, 0.5, 1, 1],
            });

            gl.bindVertexArray(this.boxesVAO);
            twgl.drawBufferInfo(gl, this.boxesBufferInfo, gl.LINES);
        }
    }
}

