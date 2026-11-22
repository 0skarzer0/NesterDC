
var isNSFPlaying = false;
var curTrack = 0;

var barColorTable = new Array(
	4278255360, // green
	4280286976,
	4282318592,
	4284350208,
	4286381824,
	4288413440,
	4290445056,
	4294967040, // yellow
	4294949376,
	4294941440,
	4294933504,
	4294925568,
	4294917632,
	4294909696,
	4294901760, // red
	4294901760
);

function visToColor(val)
{
	if (val > 3813)
		val = 3813;
	return barColorTable[Math.floor((val*15)/3813)];
}

function nsfVis(x1, y1, width, height, z)
{
	if (isNSFPlaying && Nester.nsfInfo && !Nester.frozen)
	{
		if (Nester.zeroSoundCount > 90)
		{
			curTrack++;
			stopNSF();
			Nester.nsfPlay(curTrack);
			startNSF();
		}
		var visData = Nester.visData;
		var xstep = width/6;
		var y2 = y1+height;
		for (var k = 0; k < 6; ++k)
		{
			var tempx = x1+(xstep*k);
			Video.drawRect(tempx+3, y1+(height-(height*(visData[k]/4096))), (tempx+xstep)-3, y2, 80, visToColor(visData[k]));
		}
	}
}

var g_playNSFReturnStep = null;

function startNSF()
{
	stopNSF();
	isNSFPlaying = true;
	Nester.bgNSFStart();
}

function stopNSF()
{
	if (isNSFPlaying)
	{
		isNSFPlaying = false;
		Nester.bgNSFEnd();
		Nester.clearSound();
	}
}

var g_nsf_dbEntry = null;

function padZeros2(thing)
{
	var temp = new String(thing);
	for (var k = temp.length; k < 2; ++k)
		temp = "0"+temp;
	return temp;
}

function getRunTimeStr()
{
	var seconds = Math.floor(Nester.emulatedFrames/60);
	return Math.floor(seconds/60)+":"+padZeros2(seconds%60);
}

function nsfPlayer()
{
	System.garbageCollect();
	
	print("nsfInfo.Version = "+Nester.nsfInfo.Version+"\n");
	print("nsfInfo.TotalSongs = "+Nester.nsfInfo.TotalSongs+"\n");
	print("nsfInfo.StartingSong = "+Nester.nsfInfo.StartingSong+"\n");
	print("nsfInfo.Name = "+Nester.nsfInfo.Name+"\n");
	print("nsfInfo.Artist = "+Nester.nsfInfo.Artist+"\n");
	print("nsfInfo.Copyright = "+Nester.nsfInfo.Copyright+"\n");
	
	var nsfbg = new Image();
	nsfbg.load("/cd/graphics/nsfbg.jpg", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	
	var nsfPause = new Image();
	var nsfPlay = new Image();
	var nsfStop = new Image();
	var nsfBack = new Image();
	var nsfForward = new Image();
	
	nsfPause.load("/cd/graphics/nsfpause.png", IMG_PUNCH_THRU, IMG_FILTER_BILINEAR, IMG_ALPHA_KEYED);
	nsfPlay.load("/cd/graphics/nsfplay.png", IMG_PUNCH_THRU, IMG_FILTER_BILINEAR, IMG_ALPHA_KEYED);
	nsfStop.load("/cd/graphics/nsfstop.png", IMG_PUNCH_THRU, IMG_FILTER_BILINEAR, IMG_ALPHA_KEYED);
	nsfBack.load("/cd/graphics/nsfback.png", IMG_PUNCH_THRU, IMG_FILTER_BILINEAR, IMG_ALPHA_KEYED);
	nsfForward.load("/cd/graphics/nsfforward.png", IMG_PUNCH_THRU, IMG_FILTER_BILINEAR, IMG_ALPHA_KEYED);
	
	var sliderThumb = new Image();
	sliderThumb.load("/cd/graphics/SliderThumb.png", IMG_PUNCH_THRU, IMG_FILTER_BILINEAR, IMG_ALPHA_KEYED);
	
	nsfPause.xPos = 118;
	nsfPlay.xPos = 184;
	nsfStop.xPos = 250;
	nsfBack.xPos = 63;
	nsfForward.xPos = 303;
	
	var buttonArray = new Array(nsfBack, nsfPause, nsfPlay, nsfStop, nsfForward);
	
	var pulseCounter = 1.0;
	var pulseDir = true;
	var curSel = 0;
	var pauseFlash = 0;
	curTrack = 0;
	
	var greatestBar = 0;
	
	var falloffVals = new Array();
	for (var k = 0; k < 100; ++k)
		falloffVals[k] = 0;
	
	var curLevel = 0;
	
	var sliderX = 448+77;
	
	var sliderVals = new Array();
	for (var k = 0; k < 6; ++k)
		sliderVals[k] = (Nester.getPan(k)*154)/255;
	
	var focusLevel = 6;
	
	var nicePics = new Array();
	if (g_nsf_dbEntry)
	{
		var niceTypes = new Array('titleshot', 'screenshot', 'boxscan', 'cartscan');
		for (var k = 0; k < niceTypes.length; ++k)
		{
			var curType = niceTypes[k];
			if (g_nsf_dbEntry[curType])
			{
				var tempimg = new Image();
				tempimg.load(g_extraDirs[curType]+padZeros(g_nsf_dbEntry[curType])+g_extraExts[curType], IMG_OPAQUE, IMG_FILTER_BILINEAR, IMG_ALPHA_NONE);
				nicePics[nicePics.length] = tempimg;
			}
		}
	}
	
	startNSF();
	
	var niceCount = 0;
	var curNice = 0;
	while (true)
	{
		pad1.poll();
		if (pad1.justPressed("B"))
		{
			for (var k = 0; k < nicePics.length; ++k)
				nicePics[k].unload();
			if (g_nsf_dbEntry)
				g_nsf_dbEntry = null;
			if (!isNSFPlaying)
				stopNSF();
			if (g_playNSFReturnStep)
				return g_playNSFReturnStep;
			else
				return mainMenu;
		}
		if (focusLevel == 6)
		{
			if (pad1.justPressed("A"))
			{
				switch (curSel)
				{
					case 0:
						curTrack--;
						if (curTrack < 0)
							curTrack = 0;
						stopNSF();
						Nester.nsfPlay(curTrack);
						startNSF();
						break;
					case 1:
						Nester.frozen = !Nester.frozen;
						if (Nester.frozen)
						{
							stopNSF();
							pauseFlash = 0;
						}
						else
							startNSF();
						break;
					case 2:
						Nester.frozen = false;
						stopNSF();
						Nester.nsfPlay(curTrack);
						startNSF();
						break;
					case 3:
						stopNSF();
						Nester.frozen = true;
						break;
					case 4:
						curTrack++;
						stopNSF();
						Nester.nsfPlay(curTrack);
						startNSF();
						break;
				}
			}
			if (pad1.justPressed("RIGHT"))
			{
				pulseCounter = 1;
				pulseDir = true;
				curSel++;
			}
			if (pad1.justPressed("LEFT"))
			{
				pulseCounter = 1;
				pulseDir = true;
				curSel--;
			}
		}
		else
		{
			if (pad1.LEFT)
			{
				if (sliderVals[focusLevel] > 1)
				{
					sliderVals[focusLevel]--;
					Nester.setPan(focusLevel, (sliderVals[focusLevel]*255)/154);
				}
			}
			if (pad1.RIGHT)
			{
				if (sliderVals[focusLevel] < 154)
				{
					sliderVals[focusLevel]++;
					Nester.setPan(focusLevel, (sliderVals[focusLevel]*255)/154);
				}
			}
			if (pad1.justPressed("A"))
			{
				sliderVals[focusLevel] = 77;
				Nester.setPan(focusLevel, 128);
			}
			if (pad1.justPressed("Y"))
			{
				if (focusLevel == 0)
					Nester.setPan(0, 128-50);
				else if (focusLevel == 1)
					Nester.setPan(1, 128+50);
				else
					Nester.setPan(focusLevel, 128);
				sliderVals[focusLevel] = (Nester.getPan(focusLevel)*154)/255;
			}
		}
		var oldLevel = focusLevel;
		if (pad1.justPressed("UP"))
		{
			if (focusLevel > 0)
				focusLevel--;
			else
				focusLevel = 6;
		}
		if (pad1.justPressed("DOWN"))
		{
			if (focusLevel < 6)
				focusLevel++;
			else
				focusLevel = 0;
		}
		if (oldLevel != focusLevel && focusLevel == 6)
		{
			pulseCounter = 1;
			pulseDir = true;
		}
		
		if (curSel < 0)
			curSel = buttonArray.length - 1;
		if (curSel == buttonArray.length)
			curSel = 0;
		
		Video.frameBegin();
		nsfbg.draw(0, 0, 1, g_colors["white"], 10);
		for (var k = 0; k < buttonArray.length; ++k)
			buttonArray[k].drawRotated(buttonArray[k].xPos, 429, (curSel==k && focusLevel==6) ? pulseCounter : 1.0, g_colors["white"], 15, 0.5, 0.5, 0);
		var sliderY = 51;
		for (var sliderCount = 0; sliderCount < 6; sliderCount++)
		{
			sliderThumb.drawRotated(449+sliderVals[sliderCount], sliderY, (focusLevel==sliderCount) ? pulseCounter : 1.0, g_colors["white"], 15, 0.5, 0.5, 0);
			sliderY += 43;
		}
		
		nsfVis(358, 392, 262, 72, 20);
		main_font.draw("Track: "+padZeros2(curTrack+1), 436, 311, 100, -1, 24, g_colors["white"]);
		main_font.draw("Time: "+getRunTimeStr(), 436, 341, 100, -1, 24, g_colors["white"]);
		
		if (isNSFPlaying && Nester.frozen)
		{
			if ((Math.floor(pauseFlash/30)%2) == 0)
				main_font.draw("[P A U S E D]", 371, 437, 100, -1, 30, g_colors["white"]);
			pauseFlash++;
		}
		
		if (nicePics.length > 0)
			nicePics[curNice].drawRotated(204, 198, 1, g_colors["white"], 15, 0.5, 0.5, 0);
		
		Video.frameFinish();
		
		if (focusLevel==6)
		{
			pulseCounter += pulseDir ? 0.01 : -0.01;
			if (pulseCounter > 1.2 || pulseCounter < 1.0)
				pulseDir = !pulseDir;
		}
		else
		{
			pulseCounter += pulseDir ? 0.03 : -0.03;
			if (pulseCounter > 2 || pulseCounter < 1.0)
				pulseDir = !pulseDir;
		}
		
		niceCount++;
		if ((niceCount%600)==0)
		{
			if (nicePics.length > 0)
			{
				curNice++;
				if (curNice == nicePics.length)
					curNice = 0;
			}
		}
	}
}