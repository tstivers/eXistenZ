unbind(KEY_F2);
bind(KEY_F2, toggle_physicsDebug);
unbind(KEY_F3);
bind(KEY_F3, toggle_entityAxis);


var physicsDebug = false;
function toggle_physicsDebug() {
    if (!physicsDebug) {
        system.physics.setParameter(NX_VISUALIZATION_SCALE, 1.0);
        system.physics.setParameter(NX_VISUALIZE_BODY_AXES, 1.0);
        system.physics.setParameter(NX_VISUALIZE_WORLD_AXES, 1.0);
        system.physics.setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0);
        system.physics.debugrender = 1;
        physicsDebug = true;
    }
    else {
        system.physics.setParameter(NX_VISUALIZATION_SCALE, 0.0);
        system.physics.setParameter(NX_VISUALIZE_COLLISION_SHAPES, 0.0)
        system.physics.debugrender = 0;
        physicsDebug = false;
    }
}

const EV_AXIS = 1 << 0;
const EV_BOUNDING_BOX = 1 << 1;
const EV_VELOCITY = 1 << 2;

var entityAxis = false;
function toggle_entityAxis() {
    if(!entityAxis)
    {
        system.render.setDebugFlag(EV_AXIS, true);
        entityAxis = true;
        print("axis display on")
    }
    else
    {
        system.render.setDebugFlag(EV_AXIS, false);
        entityAxis = false;
        print("axis display off")
    }
}