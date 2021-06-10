import bpy as b


selection = b.context.selected_objects

for object in selection:
    b.context.view_layer.objects.active = object
    b.ops.mesh.customdata_custom_splitnormals_clear()