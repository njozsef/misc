// Circle reslolution. Higher is smoother.
$fn=180;

injectDiam = 17;
extDiam = injectDiam + 4;
len = 20;
window_len = len - 5;
plate_width = 12;
plate_height = 3;

union() {
	difference() {
 		cylinder(d=extDiam, , h=len); 
 		cylinder(d=injectDiam, , h=len); 
		translate([0, 0, len / 2])
 			rotate([0,90,0]) 
				resize(newsize=[window_len,0,0]) 
					cylinder(d=injectDiam / 1.5, h=extDiam * 2, center = true); 
//	   translate([0, 0, len / 2]) cube([extDiam, injectDiam / 2, window_len ], center = true);
	};
   translate([-plate_width / 2, injectDiam / 2, 0]) cube([plate_width, plate_height, len]);
}
