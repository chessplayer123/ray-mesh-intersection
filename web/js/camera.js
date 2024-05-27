const M_PI_2 = 3.14 / 2;

class Camera {
    constructor(point, vector) {
        this.pos = twgl.v3.create(0, 0, 0);

        this.front = twgl.v3.create(1, 0, 0);
        this.left = twgl.v3.create(0, 0, -1);
        this.up = twgl.v3.create(0, 1, 0);

        this.yaw = 0.0;
        this.pitch = 0.0;

        this.projection = twgl.m4.perspective(2.0, 1, 0.01, 1000);
        this.view = twgl.m4.lookAt(
            this.pos,
            twgl.v3.add(this.pos, this.front),
            twgl.v3.create(0, 1, 0)
        );
    }

    prepareScene() {
        twgl.setUniforms(programInfo, {
            u_view: twgl.m4.inverse(this.view),
            u_projection: this.projection,
        });
    }

    rotate(yaw_delta_degree, pitch_delta_degree) {
        const yaw_delta = yaw_delta_degree * Math.PI / 180.0;
        const pitch_delta = pitch_delta_degree * Math.PI / 180.0;

        this.pitch = Math.max(-M_PI_2, Math.min(this.pitch + pitch_delta, M_PI_2));
        this.yaw += yaw_delta;

        twgl.v3.normalize(
            twgl.v3.create(
                Math.cos(this.yaw) * Math.cos(this.pitch),
                Math.sin(this.pitch),
                Math.sin(this.yaw) * Math.cos(this.pitch)
            ),
            this.front
        );
        twgl.v3.normalize(
            twgl.v3.create(Math.cos(this.yaw - M_PI_2), 0, Math.sin(this.yaw - M_PI_2)),
            this.left
        );
        twgl.v3.cross(this.front, this.left, this.up);

        twgl.m4.lookAt(
            this.pos,
            twgl.v3.add(this.pos, this.front),
            twgl.v3.create(0, 1, 0),
            this.view
        );
    }

    move(forward, left, up) {
        twgl.v3.add(this.pos, twgl.v3.mulScalar(this.front, forward), this.pos);
        twgl.v3.add(this.pos, twgl.v3.mulScalar(this.up, up), this.pos);
        twgl.v3.add(this.pos, twgl.v3.mulScalar(this.left, left), this.pos);

        twgl.m4.lookAt(
            this.pos,
            twgl.v3.add(this.pos, this.front),
            twgl.v3.create(0, 1, 0),
            this.view
        );
    }

    resize(w, h) {
        this.projection = twgl.m4.perspective(1.0, w / h, 1, 2000);
    }

    eyeRay() {
        return new Module.Ray(
            new Module.Vector(this.pos[0], this.pos[1], this.pos[2]),
            new Module.Vector(this.front[0], this.front[1], this.front[2]),
        );
    }
}

