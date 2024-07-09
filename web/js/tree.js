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

            const dx = mx[0] - mn[0];
            const dy = mx[1] - mn[1];
            const dz = mx[2] - mn[2];

            points.push(
                mn[0],      mn[1],      mn[2],
                mn[0] + dx, mn[1],      mn[2],
                mn[0],      mn[1],      mn[2],
                mn[0],      mn[1],      mn[2] + dz,
                mn[0] + dx, mn[1],      mn[2],
                mn[0] + dx, mn[1],      mn[2] + dz,
                mn[0],      mn[1],      mn[2] + dz,
                mn[0] + dx, mn[1],      mn[2] + dz,

                mn[0],      mn[1] + dy, mn[2],
                mn[0] + dx, mn[1] + dy, mn[2],
                mn[0],      mn[1] + dy, mn[2],
                mn[0],      mn[1] + dy, mn[2] + dz,
                mn[0],      mn[1] + dy, mn[2] + dz,
                mn[0] + dx, mn[1] + dy, mn[2] + dz,
                mn[0] + dx, mn[1] + dy, mn[2],
                mn[0] + dx, mn[1] + dy, mn[2] + dz,

                mn[0],      mn[1],      mn[2],
                mn[0],      mn[1] + dy, mn[2],
                mn[0] + dx, mn[1],      mn[2],
                mn[0] + dx, mn[1] + dy, mn[2],
                mn[0],      mn[1],      mn[2] + dz,
                mn[0],      mn[1] + dy, mn[2] + dz,
                mn[0] + dx, mn[1],      mn[2] + dz,
                mn[0] + dx, mn[1] + dy, mn[2] + dz,
            );
        }
        return points;
    }

    descendAlongRay(ray) {
        let childrenNodes = [];
        let intersections = new Module.PointsList();
        for (const node of this.nodes) {
            if (node.isLeaf()) {
                const points = ray.intersectsNode(node);
                for (let i = 0; i < points.size(); ++i) {
                    intersections.push_back(points.get(i));
                }
            } else {
                if (ray.isIntersectsAABB(node.left().box)) {
                    childrenNodes.push(node.left());
                }
                if (ray.isIntersectsAABB(node.right().box)) {
                    childrenNodes.push(node.right());
                }
            }
        }
        this.nodes = childrenNodes;
        return intersections;
    }

    ascendToRoot() {
        this.nodes = [this.data.root()];
    }
}
