unbind(KEY_Q);
bind(KEY_Q, doIt);
unbind(KEY_G)
bind(KEY_G, wakeEverything);

function wakeEverything() {
    for (name in entities) {
        if (entities[name].sleeping)
            entities[name].sleeping = false;
    }
}

function createBoxStack(pos, rot, height, width)
{
    for(i = 0; i < width; i++)
        for(j = 0; j < height; j++)
        {
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            //print("added box " + box.name);
            system.scene.addEntity(box);
            entities[box.name] = box;
            var offset = new Vector(pos);
            offset.x = (i * 1) - ((width * 1) / 2);
            offset.y = j * 1;
            offset.z = 0;
            offset.rotate(rot);
            box.setPos(offset.add(pos));
            box.setRot(rot);
        }
}

function createBoxPyramid(pos, rot, width)
{
    for(i = 0; i < width; i++)
        for(j = 0; j < width - i; j++)
        {
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            var offset = new Vector(pos);
            offset.x = ((j * 40) + (i * 20)) - ((width * 40) / 2);
            offset.y = (i * 1);
            offset.z = 0;
            offset.rotate(rot);
            box.setPos(offset.add(pos));
            box.setRot(rot);
            box.sleeping = true;
        }
}

function createBoxTower(pos, rot, length, width, height)
{
    // front and back
   for(i = 0; i < width; i++)
        for(j = 0; j < height; j++)
        {
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            var offset = new Vector(pos);
            offset.x = (i * 1) - ((width * 1) / 2);
            offset.y = j * 1;
            offset.z = 0;
            offset.rotate(rot);
            box.setPos(offset.add(pos));
            box.setRot(rot);
            box.sleeping = true;
            
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            offset = new Vector(pos);
            offset.x = (i * 1) - ((width * 1) / 2);
            offset.y = j * 1;
            offset.z = (length - 1) * 1;
            offset.rotate(rot);
            box.setPos(offset.add(pos));
            box.setRot(rot);
            box.sleeping = true;
        }
        
   for(i = 1; i < length - 1; i++)
   {
        for(j = 0; j < height; j++)
        {
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            offset = new Vector(pos);
            offset.x = - ((width * 1) / 2);
            offset.y = j * 1;
            offset.z = i * 1;
            offset.rotate(rot);
            box.setPos(offset.add(pos));
            box.setRot(rot);
            box.sleeping = true;
            
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            offset = new Vector(pos);
            offset.x = (((width - 2) * 1) / 2);
            offset.y = j * 1;
            offset.z = i * 1;
            offset.rotate(rot);
            box.setPos(offset.add(pos));
            box.setRot(rot);
            box.sleeping = true;
        }
    }
}

function createRoundTower(pos, divisions, radius, height)
{
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < divisions; j++)
        {
            var offset = new Vector(0, 0, 1);
            offset.rotate((j * (360 / divisions)) + (i * (360 / divisions) / 2), 0, 0);
            offset.mul(radius);
            offset.add(pos);
            offset.y += i * 2;
            fridge = createMeshEntity("daddy" + num_entities++, "meshes/bigdaddy.fbx#10840-mesh");
            entities[fridge.name] = fridge;
            system.scene.addEntity(fridge);
            fridge.setPos(offset);
            var rot = new Vector(0, 0, 0);
            rot.x = (j * (360 / divisions)) + (i * (360 / divisions) / 2);
            rot.y = -90;
            fridge.setrot(rot);
            fridge.sleeping = true;
        }
    }
}
            

function doIt()
{
    var pos = new Vector(game.player.getPos());
    var rot = new Vector(game.player.getRot());
    rot.y = 0; rot.z = 0;
    var offset = new Vector(0, 0, 1);
    offset.rotate(rot);
    offset.mul(20);
    pos.add(offset);
    pos.y -= (game.player.size.y / 2) + 0.001;
    //createBoxStack(pos, rot, 10, 10);
    //createBoxPyramid(pos, rot, 20);
    //createBoxTower(pos, rot, 10, 10, 20);
    createRoundTower(pos, 25, 5, 10);
}