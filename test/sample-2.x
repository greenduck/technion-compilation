/* This is a sample code */

integer function(aaa :integer, bbb :real)
var m, n :integer;
begin
	write(aaa);
	write(bbb);
	write(m);
	if (aaa > 0) then
		return(m + n);
	bbb = call(aaa - 1, bbb);
end

program
var	a, b,c:integer;
	x, y :real;
begin

	x = 2;
	x = x + y;
	y = b + c * a;
	
	if (a > b) && (x == c) then
		b = b * 2;
		
	y = call(2, b);
		
	b = b - 1;
	/* a = a + 1.0; */

end

