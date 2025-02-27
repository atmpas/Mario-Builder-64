const Collision spooner_collision[] = {
	COL_INIT(),
	COL_VERTEX_INIT(24),
	COL_VERTEX(127, 0, 0),
	COL_VERTEX(63, 0, -110),
	COL_VERTEX(63, 242, -110),
	COL_VERTEX(127, 242, 0),
	COL_VERTEX(-63, 0, -110),
	COL_VERTEX(0, 0, 0),
	COL_VERTEX(-63, 256, -110),
	COL_VERTEX(0, 256, 0),
	COL_VERTEX(63, 256, -110),
	COL_VERTEX(-127, 0, 0),
	COL_VERTEX(-127, 242, 0),
	COL_VERTEX(-63, 242, -110),
	COL_VERTEX(-63, 0, 110),
	COL_VERTEX(63, 0, 110),
	COL_VERTEX(63, 242, 110),
	COL_VERTEX(-63, 242, 110),
	COL_VERTEX(63, 256, 110),
	COL_VERTEX(-63, 256, 110),
	COL_VERTEX(127, 256, 0),
	COL_VERTEX(-127, 256, 0),
	COL_VERTEX(-41, 80, 115),
	COL_VERTEX(41, 80, 115),
	COL_VERTEX(41, 162, 115),
	COL_VERTEX(-41, 162, 115),
	COL_TRI_INIT(SURFACE_NO_CAM_COLLISION, 36),
	COL_TRI(0, 1, 2),
	COL_TRI(0, 2, 3),
	COL_TRI(4, 1, 5),
	COL_TRI(6, 7, 8),
	COL_TRI(4, 9, 10),
	COL_TRI(4, 10, 11),
	COL_TRI(12, 13, 14),
	COL_TRI(12, 14, 15),
	COL_TRI(1, 4, 11),
	COL_TRI(1, 11, 2),
	COL_TRI(13, 0, 3),
	COL_TRI(13, 3, 14),
	COL_TRI(9, 12, 15),
	COL_TRI(9, 15, 10),
	COL_TRI(16, 7, 17),
	COL_TRI(8, 7, 18),
	COL_TRI(17, 7, 19),
	COL_TRI(18, 7, 16),
	COL_TRI(19, 7, 6),
	COL_TRI(13, 12, 5),
	COL_TRI(1, 0, 5),
	COL_TRI(12, 9, 5),
	COL_TRI(0, 13, 5),
	COL_TRI(9, 4, 5),
	COL_TRI(11, 10, 19),
	COL_TRI(11, 19, 6),
	COL_TRI(10, 15, 17),
	COL_TRI(10, 17, 19),
	COL_TRI(15, 14, 16),
	COL_TRI(15, 16, 17),
	COL_TRI(14, 3, 18),
	COL_TRI(14, 18, 16),
	COL_TRI(3, 2, 8),
	COL_TRI(3, 8, 18),
	COL_TRI(2, 11, 6),
	COL_TRI(2, 6, 8),
	COL_TRI_INIT(SURFACE_DEFAULT, 2),
	COL_TRI(20, 21, 22),
	COL_TRI(20, 22, 23),
	COL_TRI_STOP(),
	COL_END()
};
