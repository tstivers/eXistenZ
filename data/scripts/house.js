print("Entering the house of 1000 clams...");

// set skybox
system.render.skybox.texture = "textures/skybox/fullmoon";
system.render.skybox.reset();

// set player position
game.player.setPos(0, 0, 0);
game.player.setRot(0, 0, 0);

// slow player down
game.player.speed = 10;

// turn on clipping
game.noclip = 0;