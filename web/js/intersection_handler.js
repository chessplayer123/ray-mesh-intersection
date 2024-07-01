const threadsCountSlider = document.getElementById("threads-count");


function toFixed(num, digitsCount=2) {
    return Number(num).toFixed(digitsCount);
}


class IntersectionHandler {
    constructor() {
        this.coords = [];
        this.info = "";

        this.ray = null;
        this.tree = null;

        this.points = null;
        this.boxes = null;
        this.line = null;

        this.boxesColor = [0.5, 0.5, 1, 1];
        this.pointsColor = [0.96, 0.3, 0.0, 1];
    }

    extendPoints(intersections) {
        const size = intersections.size();
        if (size > 0) {
            for (let i = 0; i < size; ++i) {
                this.coords.push(...intersections.get(i));
            }
            this.points = new Drawable({position: this.coords}, this.pointsColor, gl.POINTS);
        }
    }

    findIntersections(ray, tree) {
        this.clear();

        let start = performance.now();
        const intersections = ray.intersectsKDTree(tree.data);
        const seqTimeInfo = `${toFixed(performance.now() - start, 3)} ms`;

        let parTimeInfo = "-";
        if (useParallel) {
            start = performance.now();
            ray.poolIntersectsTree(tree.data, parseInt(threadsCountSlider.value));
            parTimeInfo = `${toFixed(performance.now() - start, 3)} ms`;
        }

        this.ray = ray;
        this.info = `seq: ${seqTimeInfo}, par: ${parTimeInfo}`;

        this.extendPoints(intersections);
        const pair = ray.intersectsAABB(tree.data.root().box);
        this.line = new Drawable({
            position: [ray.at(0), ray.at(Math.max(pair.first, pair.second))].flat()
        }, this.pointsColor, gl.LINES);
    }

    startTraversal(ray, tree) {
        this.clear();
        this.tree = tree;
        this.ray = ray;

        tree.ascendToRoot();

        this.boxes = new Drawable({position: this.tree.createWireframe()}, this.boxesColor, gl.LINES);
        const pair = ray.intersectsAABB(tree.data.root().box);
        this.line = new Drawable({
            position: [ray.at(0), ray.at(Math.max(pair.first, pair.second))].flat()
        }, this.pointsColor, gl.LINES);
    }

    traverse() {
        if (this.tree) {
            this.extendPoints(this.tree.descendAlongRay(this.ray));

            const wireframe = this.tree.createWireframe();
            if (wireframe.length == 0) {
                this.boxes = null;
            } else {
                this.boxes = new Drawable({position: wireframe}, this.boxesColor, gl.LINES);
            }
        }
    }

    clear() {
        this.coords = [];
        this.info = "";

        this.ray = null;
        this.tree = null;

        this.line = null;
        this.points = null;
        this.boxes = null;
    }

    draw() {
        if (this.points) this.points.draw();
        if (this.boxes)  this.boxes.draw();
        if (this.line)   this.line.draw();
    }
}

