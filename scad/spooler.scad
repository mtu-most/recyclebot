// vim: set foldmethod=marker foldmarker={,} filetype=cpp :

//{ Choose what to generate by uncommenting it.
translate ([0, 50, 0]) idler ();
translate ([0, 0, 0]) motor ();
translate ([80, 50, 0]) spoolgear ();
translate ([-80, 50, 0]) motorgear ();
translate ([80, -20, 0]) clamp ();
translate ([-80, -20, 0]) clamp ();
//}

// Distances. {
// Idler. {
	d_bearing = 22;		// Bearing diameter.  It's really 23, so it's very tight once it's forced in.
	nut = 13;		// Size of nut to use on bearing.
	d_axis = 8;
	bearing_base = 3;	// Thickness of base that bearing rests on (which contains the nut).
	base = 40;		// Width of the horizontal part.
	height = 30;		// Heigth of the horizontal part; also of other parts before rotation.
	thickness = 10;		// Thickness.
	side = 20;		// size of the side, excluding circular part.
	d_rod = 8;		// Diameter of rod holes.
	hole_offset = 16;	// Location of rod hole.
//}
// Motor. {
	axis_to_motor = 30;	// Must be enough to not let axis hit motor.
	motor_to_mount = 20;	// Motor axis to mount screws; vertically.
	mount_dist = 33.5;	// Distance between mount screws (measured 34, but that was too much).
	d_mount = 3;		// Diameter of mount screw holes.
	d_motor = 12;		// Diameter of motor axis hole.
	support_thickness = 4;	// Support plate that the motor screws onto.
	extra_length = 5;	// Mount screws to bottom.
//}
// Gears. {
	angle = 90;
	teeth_spool = 22;
	teeth_motor = 15;
	gear_thickness = 5;
	nut_trap_thickness = 3;
	d_motor_axis = 8;
	motor_flat = 5;
//}
// Spool clamp. {
	clamp_base = 4;
	clamp_teeth_start = 5;
	clamp_teeth_height = 5;
	clamp_teeth_num = 8;
	d_clamp_inner = 58;
	clamp_teeth_thickness = 3;
	teeth_gap_size = 4;
	clamp_nut_trap_thickness = 2;
//}
//}

use <gears.scad>

function nut_r (d) = (d / 2) / sin (60);

module idler () {
	translate ([-base / 2, -height / 2, 0]) difference () {
		union () {
			cube ([base, height, thickness]); // Base.
			scale ([-1, 1, 1]) rotate (30, [0, 0, 1]) { // Left side.
				cube ([side, height, thickness]);
				translate ([side, height / 2, 0]) cylinder (r = height / 2, h = thickness);
			}
			translate ([base, 0, 0]) rotate (30, [0, 0, 1]) { // Right side.
				cube ([side, height, thickness]);
				translate ([side, height / 2, 0]) cylinder (r = height / 2, h = thickness);
			}
		}
		rotate (-30, [0, 0, 1]) translate ([-hole_offset, 0, thickness / 2]) rotate (90, [1, 0, 0]) cylinder (r = d_rod / 2, h = height * 3, center = true); // Left rod hole.
		translate ([base, 0, 0]) rotate (30, [0, 0, 1]) translate ([hole_offset, 0, thickness / 2]) rotate (90, [1, 0, 0]) cylinder (r = d_rod / 2, h = height * 3, center = true); // Right rod hole.
		translate ([base / 2, height / 2, bearing_base]) cylinder (r = d_bearing / 2, h = thickness); // Bearing.
		translate ([base / 2, height / 2, thickness / 2]) cylinder (r = nut_r (nut) + 1, h = thickness + 1, center = true); // Nut on bearing.
	}
}

module motor () {
	difference () {
		union () {
			idler ();
			translate ([-base / 2, -axis_to_motor - motor_to_mount - extra_length, 0]) cube ([base, axis_to_motor + motor_to_mount + extra_length + .01 - height / 2, support_thickness]);
		}
		// motor axis hole.
		translate ([0, -axis_to_motor, -1]) cylinder (r = d_motor / 2, h = thickness + 2, $fn = 20);
		// mount holes.
		translate ([-mount_dist / 2, -axis_to_motor - motor_to_mount, -1]) cylinder (r = d_mount / 2, h = support_thickness + 2, $fn = 20);
		translate ([+mount_dist / 2, -axis_to_motor - motor_to_mount, -1]) cylinder (r = d_mount / 2, h = support_thickness + 2, $fn = 20);
	}
}

// gears {
function gear_radius (t) = t / (teeth_spool + teeth_motor) * axis_to_motor;
M_PI = atan (1) * 4;
function gear_circ (t) = gear_radius (t) * 2 * M_PI;
function tooth_dist (t) = gear_circ (t) / t;
// tan(90 - angle / 2) = twist_distance / (thickness / 2)
twist_dist = gear_thickness / 2 * tan (90 - angle / 2);
function twist_angle (t) = atan (twist_dist / gear_radius (t));
module gearbase (t) {
	// pd1 + pd2 = 2 * d
	// pd1 / pd2 = t1 / t2
	// pd = t / (t1 + t2) * 2 * d
	union () {
		gear (twist = twist_angle (t) * (gear_thickness / 2 + .001) / (gear_thickness / 2), number_of_teeth = t, diametral_pitch = t / (2 * gear_radius (t)), gear_thickness = gear_thickness / 2 + .001, rim_thickness = gear_thickness / 2 + .001, hub_thickness = gear_thickness / 2 + .001, bore_diameter = .01);
		translate ([0, 0, gear_thickness]) scale ([1, 1, -1]) gear (twist = twist_angle (t), number_of_teeth = t, diametral_pitch = t / (2 * gear_radius (t)), gear_thickness = gear_thickness / 2, rim_thickness = gear_thickness / 2, hub_thickness = gear_thickness / 2, bore_diameter = .01);
	}
}

module spoolgear () {
	difference () {
		gearbase (teeth_spool);
		translate ([0, 0, -1]) cylinder (r = d_axis / 2, h = gear_thickness + 2, $fn = 30);
		translate ([0, 0, gear_thickness - nut_trap_thickness]) cylinder (r = nut_r (nut), h = gear_thickness, $fn = 6);
	}
}

module motorgear () {
	difference () {
		gearbase (teeth_motor);
		intersection () {
			cylinder (r = d_motor_axis / 2, h = gear_thickness * 4, center = true, $fn = 30);
			cube ([motor_flat, d_motor_axis + 1, gear_thickness * 3], center = true);
		}
	}
}
//}

module clamp () {
	union () {
		difference () {
			cylinder (r = d_clamp_inner / 2 + clamp_teeth_thickness, h = clamp_teeth_start + clamp_teeth_height);
			translate ([0, 0, clamp_base]) cylinder (r = d_clamp_inner / 2, h = clamp_teeth_start + clamp_teeth_height);
			for (i = [0:clamp_teeth_num - 1]) {
				rotate (360 / clamp_teeth_num * i, [0, 0, 1]) translate ([0, -teeth_gap_size / 2, clamp_teeth_start]) cube ([clamp_teeth_thickness + d_clamp_inner + 2, teeth_gap_size, clamp_teeth_height + 1]);
			}
			translate ([0, 0, clamp_nut_trap_thickness]) cylinder (r = nut_r (nut), $fn = 6, h = clamp_base);
			translate ([0, 0, -1]) cylinder (r = d_axis / 2, h = clamp_base + 2, $fn = 30);
		}
	}
}
