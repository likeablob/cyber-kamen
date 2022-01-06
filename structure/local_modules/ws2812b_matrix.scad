WS2821_SIZE = [5, 5, 1.8];
WS2821_PITCH = [10, 10];
WS2821_EDGE_TO_CENTER = [5, 5];
WS2821_NUM_PIXEL=[8, 8];

BOARD_SIZE = [
  WS2821_PITCH.x * (WS2821_NUM_PIXEL.x - 1) + WS2821_EDGE_TO_CENTER.x * 2,
  WS2821_PITCH.y * (WS2821_NUM_PIXEL.y - 1) + WS2821_EDGE_TO_CENTER.y * 2,
  0.1
];

module translate_on_ws2812_grid(i, j) {
    translate([BOARD_SIZE.x/2, BOARD_SIZE.y/2, 0]) 
    translate([-WS2821_EDGE_TO_CENTER.x, -WS2821_EDGE_TO_CENTER.y, 0]) 
    translate([-WS2821_PITCH.x*i, -WS2821_PITCH.y*j, 0]) 
    children();
}

module ws2812b_matrix() {
  color("yellow", 0.5)
  for (i=[0:WS2821_NUM_PIXEL.x - 1]) {
    for (j=[0:WS2821_NUM_PIXEL.y - 1]) {
      translate_on_ws2812_grid(i, j)
      translate([0, 0, WS2821_SIZE.z/2]) 
      cube(size=WS2821_SIZE, center=true);
    }
  }

  translate([0, 0, BOARD_SIZE.z/2]) 
  cube(size=BOARD_SIZE, center=true);
}
