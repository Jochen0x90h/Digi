sub quaternionRotateX
{
	my @q = ("sx", "0", "0", "cx");
	return @q;
}

sub quaternionRotateY
{
	my @q = ("0", "sy", "0", "cy");
	return @q;
}

sub quaternionRotateZ
{
	my @q = ("0", "0", "sz", "cz");
	return @q;
}

sub add
{
	my $a = $_[0];
	my $b = $_[1];
	
	if ($a eq "0")
	{
		return $b;
	}
	if ($b eq "0")
	{
		return $a;
	}
	return "$a + $b";
}

sub subtract
{
	my $a = $_[0];
	my $b = $_[1];
	
	if ($b eq "0")
	{
		return $a;
	}
	if ($a eq "0")
	{
		return "-$b";
	}
	return "$a - $b";
}

sub mul
{
	my $a = $_[0];
	my $b = $_[1];
	
	if ($a eq "0")
	{
		return $a;
	}
	if ($b eq "0")
	{
		return $b;
	}
	if ($a eq "1")
	{
		return $b;
	}
	if ($b eq "1")
	{
		return $a;
	}
	return "$a * $b";
}

sub qmul
{
	my $ax = $_[0];
	my $ay = $_[1];
	my $az = $_[2];
	my $aw = $_[3];

	my $bx = $_[4];
	my $by = $_[5];
	my $bz = $_[6];
	my $bw = $_[7];
	
#	aw * bx + ax * bw + ay * bz - az * by,
#	aw * by - ax * bz + ay * bw + az * bx,
#	aw * bz + ax * by - ay * bx + az * bw,
#	aw * bw - ax * bx - ay * by - az * bz);
    
	my @q = (
		subtract(add(add(mul($aw, $bx), mul($ax, $bw)), mul($ay, $bz)), mul($az, $by)),
		add(add(subtract(mul($aw, $by), mul($ax, $bz)), mul($ay, $bw)), mul($az, $bx)),
		add(subtract(add(mul($aw, $bz), mul($ax, $by)), mul($ay, $bx)), mul($az, $bw)),
		subtract(subtract(subtract(mul($aw, $bw), mul($ax, $bx)), mul($ay, $by)), mul($az, $bz)));
	return @q;
}


@orders = ("XYZ", "YZX", "ZXY", "XZY", "YXZ", "ZYX");

for ($o = 0; $o <= $#orders; $o++)
{
	$order = $orders[$o];

	# multiply the rotations in given order
	@q = ("0", "0", "0", "1");
	for ($i = 0; $i < length($order); $i++)
	{
		$axis = substr($order, $i, 1);
		if ($axis eq "X")
		{
			@q = qmul(quaternionRotateX(), @q);
		}
		if ($axis eq "Y")
		{
			@q = qmul(quaternionRotateY(), @q);
		}
		if ($axis eq "Z")
		{
			@q = qmul(quaternionRotateZ(), @q);
		}
	}

	# write result to file
	open (f, ">quaternionEuler$order.h");
	printf f "$q[0],\n$q[1],\n$q[2],\n$q[3]);";
	close f;
}

