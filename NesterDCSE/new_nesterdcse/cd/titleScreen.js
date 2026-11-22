
function titleScreen()
{
	var titlebg = new Image();
	titlebg.load("/cd/graphics/title/title_screen.png", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	
	Video.frameBegin();
	titlebg.draw(-5, 0, 1, g_colors["white"], 10);
	main_font.draw("Loading...", 50, 50, 100, -1, 30, g_colors["white"]);
	Video.frameFinish();
	
	ramCache("/cd/ram");
	println('nesdb success = '+g_nesDB.open("/ram/mynes.db"));
	
	var wheelFiles = FileSystem.getFileList("/cd/graphics/title/wheel");
	if (wheelFiles.length == 0)
		return;
	var wheelSprites = new Array();
	for (var k = 0; k < wheelFiles.length; ++k)
	{
		if (!wheelFiles[k].isDir)
			continue;
		var tempspr = new Sprite();
		if (!tempspr.load(wheelFiles[k].fullPath+"/sprite.xml"))
			continue;
		tempspr.pivotv = (400 / (tempspr.img.getHeight() * 2)) + 1;
		tempspr.scale = 2;
		tempspr.x = 320;
		tempspr.y = 240;
		tempspr.z = 15;
		wheelSprites[wheelSprites.length] = tempspr;
	}
	
	var angle = 0;
	var pi2 = (Math.PI*2);
	var frameCount = 0;
	var leaving = false;
	var currentDistance = 400;
	while (true)
	{
		pad1.poll();
		if (pad1.justPressed("START"))
			leaving = true;
		angle += 0.01;
		if (angle > pi2)
			angle -= pi2;
		
		for (var k = 0; k < wheelSprites.length; ++k)
			wheelSprites[k].angle = angle + ((pi2/wheelSprites.length)*k);
		
		Video.frameBegin();
		titlebg.draw(-5, 0, 1, g_colors["white"], 10);
		main_font.draw("Press Start", 50, 50, 100, -1, 30, g_colors["white"]);
		Video.frameFinish();
		
		if (leaving && currentDistance <= 400)
		{
			currentDistance += 5;
			if (currentDistance > 400)
				break;
			for (var k = 0; k < wheelSprites.length; ++k)
				wheelSprites[k].pivotv = (currentDistance / (wheelSprites[k].img.getHeight() * 2)) + 1;
		}
		else if (currentDistance > 160)
		{
			currentDistance -= 5;
			for (var k = 0; k < wheelSprites.length; ++k)
				wheelSprites[k].pivotv = (currentDistance / (wheelSprites[k].img.getHeight() * 2)) + 1;
		}
		frameCount++;
	}
}