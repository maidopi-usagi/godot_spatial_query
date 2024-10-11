extends Node2D

@onready var quad_tree: QuadTreeQuery = QuadTreeQuery.new()
@onready var icon: Texture = load("res://icon.svg")

var mouse_pos := Vector2.ZERO

func _ready() -> void:
	var res := get_viewport_rect().size
	quad_tree.set_bound(0.0, 0.0, res.x, res.y)

func _process(delta: float) -> void:
	if Input.is_action_just_pressed("ui_accept"):
		var res := get_viewport_rect().size
		for i in 100:
			quad_tree.add_point("test%d"%i, Vector2(randf_range(0.0, res.x), randf_range(0.0, res.y)))
	mouse_pos = get_global_mouse_position()
	queue_redraw()
	pass

func _draw() -> void:
	for pt in quad_tree.get_all_positions():
		draw_texture_rect(icon, Rect2(pt, 16 * Vector2.ONE), false)
	for rect in quad_tree.get_all_rects():
		draw_rect(rect, Color.RED, false, 2.0)
	#for pt in quad_tree.find_neighbors(mouse_pos, 100, 100):
		#var value = quad_tree.get_point_at(pt)
		#var pos = quad_tree.get_position_at(pt)
		#draw_string(ThemeDB.fallback_font, pos, str(value), HORIZONTAL_ALIGNMENT_LEFT, -1, 16)
	pass
