class Tree {
    constructor(data) {
        this.data = data;
        this.nodes = [this.data.root()];
    }

    createWireframe() {
        let points = [];
        for (const node of this.nodes) {
            const mn = node.box.min;
            const mx = node.box.max;

            const dx = mx.x - mn.x;
            const dy = mx.y - mn.y;
            const dz = mx.z - mn.z;

            points.push(
                mn.x,      mn.y,      mn.z,
                mn.x + dx, mn.y,      mn.z,
                mn.x,      mn.y,      mn.z,
                mn.x,      mn.y,      mn.z + dz,
                mn.x + dx, mn.y,      mn.z,
                mn.x + dx, mn.y,      mn.z + dz,
                mn.x,      mn.y,      mn.z + dz,
                mn.x + dx, mn.y,      mn.z + dz,

                mn.x,      mn.y + dy, mn.z,
                mn.x + dx, mn.y + dy, mn.z,
                mn.x,      mn.y + dy, mn.z,
                mn.x,      mn.y + dy, mn.z + dz,
                mn.x,      mn.y + dy, mn.z + dz,
                mn.x + dx, mn.y + dy, mn.z + dz,
                mn.x + dx, mn.y + dy, mn.z,
                mn.x + dx, mn.y + dy, mn.z + dz,

                mn.x,      mn.y,      mn.z,
                mn.x,      mn.y + dy, mn.z,
                mn.x + dx, mn.y,      mn.z,
                mn.x + dx, mn.y + dy, mn.z,
                mn.x,      mn.y,      mn.z + dz,
                mn.x,      mn.y + dy, mn.z + dz,
                mn.x + dx, mn.y,      mn.z + dz,
                mn.x + dx, mn.y + dy, mn.z + dz,
            );
        }
        return points;
    }

    descendAlongRay(ray) {
        let childrenNodes = [];
        let intersections = new Module.PointsList();
        for (const node of this.nodes) {
            if (node.isLeaf()) {
                const points = node.intersects(ray);
                for (let i = 0; i < points.size(); ++i) {
                    intersections.push_back(points.get(i));
                }
            } else {
                const children = node.children();
                for (let i = 0; i < children.size(); ++i) {
                    if (children.get(i).box.intersects(ray)) {
                        childrenNodes.push(children.get(i));
                    }
                }
            }
        }
        this.nodes = childrenNodes;
        return intersections;
    }

    ascendToRoot() {
        this.nodes = [this.data.root()];
    }

    intersects(ray) {
        return this.data.intersects(ray);
    }

    par_intersects(ray, threadsCount) {
        return this.data.par_intersects(ray, threadsCount);
    }
}
