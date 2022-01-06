include <scad-utils/morphology.scad>

include <local_modules/ws2812b_matrix.scad>

$fn=50;

DIFFUSER_CELL_SIZE = [
  9, // WS2821_PITCH.x - 1,
  9, // WS2821_PITCH.y - 1,
  2.1,
];
DIFFUSER_CELL_TOP_EDGE_WIDTH = 0.5;
DIFFUSER_CELL_GAP_FOR_WS2812B = 0.15;
DIFFUSER_CELL_POS_Z = 1.4;

TAB_SIZE = [
  1, 
  1,
  0.6,
];

module diffuser_cell(args) {
  translate([0, 0, DIFFUSER_CELL_POS_Z])
  difference() {
    // Base shape
    linear_extrude(height=DIFFUSER_CELL_SIZE.z, center=!true, convexity=10, twist=0)
    square(size=[DIFFUSER_CELL_SIZE.x, DIFFUSER_CELL_SIZE.y], center=true);
    
    // Trapezoid
    hull() {
      // Top plane
      translate([0, 0, DIFFUSER_CELL_SIZE.z])
      linear_extrude(height=0.01, center=true, convexity=10, twist=0)
      offset(delta=-DIFFUSER_CELL_TOP_EDGE_WIDTH)
      square(size=[DIFFUSER_CELL_SIZE.x, DIFFUSER_CELL_SIZE.y], center=true);

      // Bottom plane
      linear_extrude(height=0.01, center=true, convexity=10, twist=0)
      square(size=[WS2821_SIZE.x, WS2821_SIZE.y], center=true);
    }
    
    // Window for WS2812 with some gap
    linear_extrude(height=2*2, center=!true, convexity=10, twist=0)
    offset(delta=DIFFUSER_CELL_GAP_FOR_WS2812B)
    square(size=[WS2821_SIZE.x, WS2821_SIZE.y], center=true);
  }
}

module cyber_kamen_diffuser() {
  for (i=[0:WS2821_NUM_PIXEL.x - 1]) {
    for (j=[0:WS2821_NUM_PIXEL.y - 1]) {
      translate_on_ws2812_grid(i, j){
        // A cell
        diffuser_cell();

        // Add "tabs" between cells
        PLUS_Y = 0;
        MINUS_X = 1;
        MINUS_Y = 2;
        PLUS_X = 3;
        IND_I_PLUS_X_EDGE = 0;
        IND_I_MINUS_X_EDGE = WS2821_NUM_PIXEL.x - 1;
        IND_J_PLUS_Y_EDGE = 0;
        IND_J_MINUS_Y_EDGE = WS2821_NUM_PIXEL.y - 1;

        for (tab_direction=[0:3]){
          // Skip tabs on the edges
          if ((i == IND_I_PLUS_X_EDGE && tab_direction == PLUS_X)
              || (i == IND_I_MINUS_X_EDGE && tab_direction == MINUS_X)
              || (j == IND_J_MINUS_Y_EDGE && tab_direction == MINUS_Y)
              || (j == IND_J_PLUS_Y_EDGE && tab_direction == PLUS_Y)
          ) {
            // nop
          } else {
            rotate([0, 0, 90*tab_direction])
            translate([0, WS2821_PITCH.y/2, DIFFUSER_CELL_POS_Z + TAB_SIZE.z/2])
            cube(size=TAB_SIZE, center=true);
          }
        }
      }
    }
  }
}


%
ws2812b_matrix();

cyber_kamen_diffuser();