game.player.pos.x = 675;
game.player.pos.y = 35;
game.player.pos.z = 2200;
game.player.rot.x = 180;
game.noclip = 0;


for(z = -2000; z <= 0; z += 200)
{
	for(x = 2000; x >= 0; x -= 200)
	{	
		bleh = createStaticEntity("tombstone_" + x + "_" + z, "meshes/tombstone1")
		system.scene.addEntity(bleh);
		bleh.scale.x = -1
		bleh.scale.y = -1
		bleh.scale.z = -1
		bleh.rot.x = 180
		bleh.pos.x = x;
		bleh.pos.z = z;
		bleh.update()
	}
}

bleh = createStaticEntity("ground", "meshes/groundshape1")
bleh.scale.x = -1
bleh.scale.y = -1
bleh.scale.z = -1
bleh.rot.x = 180
bleh.pos.y = 2050
bleh.update()
system.scene.addEntity(bleh)