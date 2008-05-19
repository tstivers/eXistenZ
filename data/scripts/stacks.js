unbind(KEY_Q);
bind(KEY_Q, doIt);

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
            box.pos = offset.add(pos);
            box.rot = rot;
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
            box.pos = offset.add(pos);
            box.rot = rot;
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
            box.pos = offset.add(pos);
            box.rot = rot;
            box.sleeping = true;
            
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            offset = new Vector(pos);
            offset.x = (i * 1) - ((width * 1) / 2);
            offset.y = j * 1;
            offset.z = (length - 1) * 1;
            offset.rotate(rot);
            box.pos = offset.add(pos);
            box.rot = rot;
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
            box.pos = offset.add(pos);
            box.rot = rot;
            box.sleeping = true;
            
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            offset = new Vector(pos);
            offset.x = (((width - 2) * 1) / 2);
            offset.y = j * 1;
            offset.z = i * 1;
            offset.rotate(rot);
            box.pos = offset.add(pos);
            box.rot = rot;
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
            offset.y += i * 1;
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            box.pos = offset;
            box.rot.x = (j * (360 / divisions)) + (i * (360 / divisions) / 2);
            box.sleeping = true;
        }
    }
}
            

function doIt()
{
    var pos = new Vector(game.player.pos);
    var rot = new Vector(game.player.rot);
    rot.y = 0; rot.z = 0;
    var offset = new Vector(0, 0, 1);
    offset.rotate(rot);
    offset.mul(20);
    pos.add(offset);
    pos.y -= 15 * 0.03;
    //createBoxStack(pos, rot, 10, 10);
    //createBoxPyramid(pos, rot, 20);
    //createBoxTower(pos, rot, 10, 10, 20);
    createRoundTower(pos, 60, 10, 20);
}