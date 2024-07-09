const M_PI_2 = 3.14 / 2;

class Camera {
    constructor(point, vector) {
        this.pos = twgl.v3.create(0, 0, 0);

        this.front = twgl.v3.create(1, 0, 0);
        this.left = twgl.v3.create(0, 0, -1);
        this.up = twgl.v3.create(0, 1, 0);

        this.fov = 60.0 * Math.PI / 180.0;

        this.projection = twgl.m4.perspective(this.fov, 1, 0.1, 1000);
        this.view = twgl.m4.inverse(twgl.m4.lookAt(
            this.pos,
            twgl.v3.add(this.pos, this.front),
            this.up
        ));
    }

    projectToScreen(point) {
        const mat = twgl.m4.multiply(this.projection, this.view);
        const transformed = [0.0, 0.0, 0.0, 0.0];
        point.push(1.0);

        for (let rw = 0; rw < 4; ++rw) {
            for (let cl = 0; cl < 4; ++cl) {
                transformed[rw] += point[cl] * mat[cl * 4 + rw];
            }
        }

        return [
            (transformed[0] / transformed[3] + 1) * ui.width * 0.5,
            (-transformed[1] / transformed[3] + 1) * ui.height * 0.5,
            transformed[2]
        ];
    }

    prepareScene() {
        twgl.setUniforms(programInfo, {
            u_view: this.view,
            u_projection: this.projection,
        });
    }

    updateView() {
        twgl.m4.inverse(
            twgl.m4.lookAt(
                this.pos,
                twgl.v3.add(this.pos, this.front),
                this.up
            ),
            this.view
        );
    }

    rotate(yaw_delta_degree, pitch_delta_degree, roll_delta_degree) {
        const yaw_delta = yaw_delta_degree * Math.PI / 180.0;
        const pitch_delta = pitch_delta_degree * Math.PI / 180.0;
        const roll_delta = roll_delta_degree * Math.PI / 180.0;

        const rot = twgl.m4.axisRotation(this.up, yaw_delta);
        twgl.m4.axisRotate(rot, this.left, pitch_delta, rot);
        twgl.m4.axisRotate(rot, this.front, roll_delta, rot);

        twgl.m4.transformDirection(rot, this.front, this.front);
        twgl.m4.transformDirection(rot, this.up, this.up);
        twgl.m4.transformDirection(rot, this.left, this.left);

        this.updateView();
    }

    move(forward, left, up) {
        twgl.v3.add(this.pos, twgl.v3.mulScalar(this.front, forward), this.pos);
        twgl.v3.add(this.pos, twgl.v3.mulScalar(this.up, up), this.pos);
        twgl.v3.add(this.pos, twgl.v3.mulScalar(this.left, left), this.pos);

        this.updateView();
    }

    resize(w, h) {
        twgl.m4.perspective(this.fov, w / h, 0.1, 1000, this.projection);
    }

    eyeRay() {
        return new Module.Ray(this.pos, this.front);
    }
}

