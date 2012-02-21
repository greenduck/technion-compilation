/* Sample 5
 * a^b calculation
 */

integer function(a :integer, b :integer)
var	c :integer;
begin
	if (b > 0) then
	begin
		c = call(a, b - 1);
		return(a * c);
	end
	return(1);
end

program
var	m, n :integer;
begin
	read(m);
	read(n);
	n = call(m, n);
	write(n);
end

