/* Sample 4
 * Factorial calculation
 */

integer function(a :integer, x :real)
var	b :integer;
begin
	if (a > 0) then
	begin
		b = call(a - 1, x);
		return(a * b);
	end
	return(1);
end

program
var	m, n :integer;
begin
	read(m);
	n = call(m, 0);
	write(n);
end

