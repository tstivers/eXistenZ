unbind(KEY_M);
bind(KEY_M, createFridge);
unbind(KEY_B);
bind(KEY_B, createBigDaddy);

function createFridge() {
    fridge = createMeshEntity("fridge" + num_entities++, "meshes/fridge.fbx#Fridge01");
    entities[fridge.name] = fridge;
    system.scene.addEntity(fridge);
    fridge.pos = new Vector(game.player.pos);
    fridge.rot = new Vector(game.player.rot);
    fridge.rot.y = -90;
    fridge.pos.y -= game.player.size.y;
}

function createBigDaddy() {
    fridge = createMeshEntity("daddy" + num_entities++, "meshes/bigdaddy.fbx#10840-mesh");
    entities[fridge.name] = fridge;
    system.scene.addEntity(fridge);
    fridge.pos = new Vector(game.player.pos);
    fridge.rot = new Vector(game.player.rot);
    fridge.rot.y = -90;
    fridge.rot.x -= 90;
}
