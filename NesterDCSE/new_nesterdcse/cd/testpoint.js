function TestPoint(x, y)
{
	this.x = x;
	this.y = y;
}

TestPoint.prototype.processInput = function()
{
	if (pad1.LEFT)
		this.x--;
	if (pad1.RIGHT)
		this.x++;
	if (pad1.UP)
		this.y--;
	if (pad1.DOWN)
		this.y++;
	println("test_x="+this.x+" test_y="+this.y);
}
