print("Entering the house of 1000 clams...");

// set skybox
system.render.skybox.texture = "textures/skybox/fullmoon";
system.render.skybox.reset();

// set player position
game.player.pos.x = -360;
game.player.pos.y = 20;
game.player.pos.z = -800;
game.player.rot.x = 30;
game.player.rot.y = 0;
game.player.rot.z = 0;

// slow player down
game.player.speed = 0.5;

// turn on clipping
game.noclip = 0;