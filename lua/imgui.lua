local M = {}

--IMGUI Layer that provides default parameters etc. For your sanity.
--Forwarded functions are on a single line. (with some exceptions like push/pop, and multiple possible default vals) 
--Other functions have multi line bodies.

--Internal Constants
local INT_SLIDER_MIN = -2147483648
local INT_SLIDER_MAX = 2147483647
local FLOAT_SLIDER_MIN = -3.40282347e+38 / 2
local FLOAT_SLIDER_MAX = 3.40282347e+38 / 2

--Helpers
local function boolDefault(val, def)
  if val == nil then return def end
  if type(val) ~= 'boolean' then return def end
  return val
end

local function nullableString(str)
  return str or ""
end

M.BeginRadioButtonGroup = function(sid, startId)
  M.radioButtonGroupOpen = true
  M.radioButtonGroupSid = sid
  M.radioButtonGroupId = startId or 1
end

M.EndRadioButtonGroup = function()
  M.radioButtonGroupOpen = false
  return M.radioButtonGroupSid
end

--IMGUI
M.BeginTooltip =                      function()               imgui.BeginTooltip()                                    end
M.EndTooltip =                        function()               imgui.EndTooltip()                                      end
                    
M.BeginMenu =                         function(mNam, enable)   return imgui.BeginMenu(mNam, boolDefault(enable, true)) end
M.EndMenu =                           function()               imgui.EndMenu()                                         end
M.BeginMenuBar =                      function()               return imgui.BeginMenuBar()                             end
M.EndMenuBar =                        function()               imgui.EndMenuBar()                                      end
M.BeginMainMenuBar =                  function()               return imgui.BeginMainMenuBar()                         end
M.EndMainMenuBar =                    function()               imgui.EndMainMenuBar()                                  end
          
M.Dummy =                             function(sz)             imgui.Dummy(sz or ImVec2(0, 0))                         end        
          
M.HelpMarker =                        function(text)           imgui.HelpMarker(text)                                  end
M.Separator =                         function()               imgui.Separator()                                       end
                
M.AlignTextToFramePadding =           function()               imgui.AlignTextToFramePadding()                         end
M.GetTextLineHeight =                 function()               return imgui.GetTextLineHeight()                        end
M.CalcItemWidth =                     function()               return imgui.CalcItemWidth()                            end
M.GetFrameCount =                     function()               return imgui.GetFrameCount()                            end
M.GetFrameHeight =                    function()               return imgui.GetFrameHeight()                           end
M.GetFrameHeightWithSpacing =         function()               return imgui.GetFrameHeightWithSpacing()                end
M.GetFontSize =                       function()               return imgui.GetFontSize()                              end
M.Text =                              function(text)           imgui.Text(text)                                        end
M.TextWrapped =                       function(text)           imgui.TextWrapped(text)                                 end
M.TextColored =                       function(color, text)    imgui.TextColored(color, text)                          end
M.LabelText =                         function(label, text)    imgui.LabelText(label, text)                            end
M.BulletText =                        function(text)           imgui.BulletText(text)                                  end
        
M.GetMousePos =                       function()               return imgui.GetMousePos()                              end        
M.GetMousePosOnOpeningCurrentPopup =  function()               return imgui.GetMousePosOnOpeningCurrentPopup()         end        
M.IsMouseDown =                       function(btn)            return imgui.IsMouseDown(btn)                           end
M.IsMouseClicked =                    function(btn,rp)         return imgui.IsMouseClicked(btn, boolDefault(rp,false)) end
M.IsMouseReleased =                   function(btn)            return imgui.IsMouseReleased(btn)                       end
M.IsAnyMouseDown =                    function()               return imgui.IsAnyMouseDown()                           end
M.IsMouseDragging =                   function(btn, lockThrsh) return imgui.IsMouseDragging(btn. lockThrsh or -1.0)    end
M.GetMouseDragDelta =                 function(btn, lockThrsh) return imgui.GetMouseDragDelta(btn, lockThrsh or -1.0)  end

M.IsWindowFocused =                   function(focusFlags)     return imgui.IsWindowFocused(focusFlags or 0)           end
M.IsWindowHovered =                   function(hoverFlags)     return imgui.IsWindowHovered(hoverFlags or 0)           end
M.IsItemActive =                      function()               return imgui.IsItemActive()                             end
M.IsItemFocused =                     function()               return imgui.IsItemFocused()                            end
M.IsItemHovered =                     function(hoverFlags)     return imgui.IsItemHovered(hoverFlags or 0)             end
M.IsAnyItemActive =                   function()               return imgui.IsAnyItemActive()                          end
M.IsAnyItemFocused =                  function()               return imgui.IsAnyItemFocused()                         end
M.IsAnyItemHovered =                  function()               return imgui.IsAnyItemHovered()                         end
M.IsItemActivated =                   function()               return imgui.IsItemActivated()                          end
M.IsItemVisible =                     function()               return imgui.IsItemVisible()                            end
M.IsItemDeactivated =                 function()               return imgui.IsItemDeactivated()                        end
M.IsItemDeactivatedAfterEdit =        function()               return imgui.IsItemDeactivatedAfterEdit()               end  
M.IsItemClicked =                     function(btn)            return imgui.IsItemClicked(btn)                         end
M.IsItemEdited =                      function()               return imgui.IsItemEdited()                             end
M.IsItemToggledOpen =                 function()               return imgui.IsItemToggledOpen()                        end

M.GetItemRectMin =                    function()               return imgui.GetItemRectMin()                           end
M.GetItemRectMax =                    function()               return imgui.GetItemRectMax()                           end
M.GetItemRectSize =                   function()               return imgui.GetItemRectSize()                          end

M.GetScrollX =                        function()               return imgui.GetScrollX()                               end
M.GetScrollY =                        function()               return imgui.GetScrollY()                               end
M.GetScrollMaxX =                     function()               return imgui.GetScrollMaxX()                            end
M.GetScrollMaxY =                     function()               return imgui.GetScrollMaxY()                            end
M.SetScrollHereX =                    function(xr)             return imgui.SetScrollHereX(xr or 0.5)                  end
M.SetScrollHereY =                    function(yr)             return imgui.SetScrollHereY(yr or 0.5)                  end
M.SetScrollFromPosX =                 function(lx, xr)         return imgui.SetScrollFromPosX(lx, xr or 0.5)           end
M.SetScrollFromPosY =                 function(ly, yr)         return imgui.SetScrollFromPosY(ly, yr or 0.5)           end

M.TreePop =                           function()               imgui.TreePop()                                         end
M.TreeNode =                          function(label)          return imgui.TreeNode(label)                            end
                
M.PopButtonRepeat =                   function()               imgui.PopButtonRepeat()                                 end
M.PushButtonRepeat =                  function(rp)             imgui.PushButtonRepeat(boolDefault(rp, true))           end
          
M.PushItemWidth =                     function(val)            imgui.PushItemWidth(val)                                end
M.PopItemWidth =                      function()               imgui.PopItemWidth()                                    end
                              
M.PushStyleColor =                    function(id, val)        imgui.PushStyleColor(id, val)                           end
M.PopStyleColor =                     function(c)              imgui.PopStyleColor(c or 1)                             end
                
M.SetTooltip =                        function(tooltip)        imgui.SetTooltip(tooltip)                               end
                
M.CenterNextWindow =                  function()               imgui.CenterNextWindow()                                end
M.SetItemDefaultFocus =               function()               imgui.SetItemDefaultFocus()                             end
M.SetNextItemWidth =                  function(width)          imgui.SetNextItemWidth(width)                           end
M.SetNextWindowBgAlpha =              function(alpha)          imgui.SetNextWindowBgAlpha(alpha)                       end
M.SetNextWindowContentSize =          function(sz)             imgui.SetNextWindowContentSize(sz)                      end

M.GetCursorStartPos =                 function()               return imgui.GetCursorStartPos()                        end
M.GetCursorScreenPos =                function()               return imgui.GetCursorScreenPos()                       end
M.GetCursorPos =                      function()               return imgui.GetCursorPos()                             end
M.GetCursorPosX =                     function()               return imgui.GetCursorPosX()                            end
M.GetCursorPosY =                     function()               return imgui.GetCursorPosY()                            end
M.SetCursorPos =                      function(pos)            imgui.SetCursorPos(pos)                                 end
M.SetCursorPosX =                     function(x)              imgui.SetCursorPosX(x)                                  end
M.SetCursorPosY =                     function(y)              imgui.SetCursorPosY(y)                                  end

--
M.CalcTextSize = function(text, text_end, hide_text_after_double_hash, wrap_width)
  text_end = nullableString(text_end)
  hide_text_after_double_hash = boolDefault(hide_text_after_double_hash, false)
  wrap_width = wrap_width or -1.0
  return imgui.CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width)
end


M.PopTextWrapPos = function()
  imgui.PopTextWrapPos()
end

M.PushTextWrapPos = function(wrap_local_pos_x)
  wrap_local_pos_x = wrap_local_pos_x or 0
  imgui.PushTextWrapPos(wrap_local_pos_x)
end

M.PushStyleVar = function(id, val)  
  --check what variant of the function to use
  if type(val) == 'number' then
    imgui.PushStyleVarFloat(id, val)
  else
    imgui.PushStyleVarVec2(id, val)
  end
end

M.PopStyleVar = function(c)
  imgui.PopStyleVar(c or 1)                               
end

M.BeginTabItem = function(label ,flags)
  flags = flags or 0
  return imgui.BeginTabItem(label, flags)
end

M.EndTabItem = function()               
  imgui.EndTabItem()                                      
end

M.BeginTabBar = function(str_id, flags)              
  str_id = str_id or "##TabBar"
  flags = flags or 0
  return imgui.BeginTabBar(str_id, flags)
end

M.EndTabBar = function()               
  imgui.EndTabBar()                                      
end

local function plotLinesHistogramSharedSig(label, values, valuesOffset, overlayText, scaleMin, scaleMax, size, stride, func)
  --One scary function signature..
  valuesOffset = valuesOffset or 0
  overlayText = nullableString(overlayText)
  scaleMin = scaleMin or 3.402823466e+38
  scaleMax = scaleMax or 3.402823466e+38
  size = size or ImVec2(0.0, 0.0)
  stride = stride or 4
  func(label, values, valuesOffset, overlayText, scaleMin, scaleMax, size, stride)
end

M.PlotLines = function(label, values, valuesOffset, overlayText, scaleMin, scaleMax, size, stride)
  plotLinesHistogramSharedSig(label, values, valuesOffset, overlayText, scaleMin, scaleMax, size, stride, imgui.PlotLines)
end

M.PlotHistogram = function(label, values, valuesOffset, overlayText, scaleMin, scaleMax, size, stride)
  plotLinesHistogramSharedSig(label, values, valuesOffset, overlayText, scaleMin, scaleMax, size, stride, imgui.PlotHistogram)
end

M.InputFloat = function(label, value, step, stepFast, flags)
  step = step or 0
  stepFast = stepFast or 0
  flags = flags or 0
  return imgui.InputFloat(label, value, step, stepFast, flags)
end

M.InputInt = function(label, value, step, stepFast, flags)
  step = step or 1
  stepFast = stepFast or 100
  flags = flags or 0
  return imgui.InputInt(label, value, step, stepFast, flags)
end

M.InputText = function(label, text, flags)
  flags = flags or 0
  return imgui.InputText(label, text, flags)
end

M.InputTextMultiline = function(label, text, size, flags)
  size = size or ImVec2(0.0, 0.0)
  flags = flags or 0
  return imgui.InputTextMultiline(label, text, size, flags)
end

M.InputTextWithHint = function(label, hint, text, flags)
  flags = flags or 0
  return imgui.InputTextWithHint(label, hint, text, flags)
end

M.SetNextWindowPos = function(pos, cond, pivot)
  pivot = pivot or ImVec2(0.0, 0.0)
  cond = cond or 0
  imgui.SetNextWindowPos(pos, cond, pivot)
end

M.SetNextWindowSize = function(sz, cond)
  cond = cond or 0
  imgui.SetNextWindowSize(sz, cond)
end

M.BeginChild = function(str_id, size, border, flags)
  flags = flags or 0
  border = boolDefault(border, true)
  size = size or ImVec2(0, 100)
  return imgui.BeginChild(str_id, size, border, flags)
end

M.EndChild = function()               
  imgui.EndChild()                        
end

M.BeginPopupModal = function(name, open, flags)
  flags = flags or 0
  open = boolDefault(open, true)
  return imgui.BeginPopupModal(name, open, flags)
end

M.EndPopup = function()               
  imgui.EndPopup()                        
end

M.CloseCurrentPopup = function()               
  imgui.CloseCurrentPopup()                        
end

M.OpenPopup = function(name, flags)               
  flags = flags or 0
  imgui.OpenPopup(name, flags)                       
end

M.Selectable = function(label, selected, flags, size)
  selected = boolDefault(selected, false)
  flags = flags or 0
  size = size or ImVec2(0.0, 0.0)
  
  --Multi return: clicked, selected
  return imgui.Selectable(label, selected, flags, size)
end

M.MenuItem = function(label, shortcut, selected, enabled)
  selected = boolDefault(selected, false)
  enabled = boolDefault(enabled, true)
  shortcut = nullableString(shortcut)
  
  --Multi return: clicked, selected
  return imgui.MenuItem(label, shortcut, selected, enabled)
end

M.BeginPopupContextItem = function(str_id, flags)
  return imgui.BeginPopupContextItem(nullableString(str_id), flags or 0)
end

M.BeginPopupContextWindow = function(str_id, flags)
  return imgui.BeginPopupContextWindow(nullableString(str_id), flags or 1)
end

M.BeginPopupContextVoid = function(str_id, flags)
  return imgui.BeginPopupContextVoid(nullableString(str_id), flags or 1)
end

M.EndPopup = function()
  imgui.EndPopup()
end

M.LeftArrowButton = function()
  return imgui.LeftArrowButton()
end

M.RightArrowButton = function()
  return imgui.RightArrowButton()
end

M.UpArrowButton = function()
  return imgui.UpArrowButton()
end


M.DownArrowButton = function()
  return imgui.DownArrowButton()
end

M.Unindent = function(indent_w)
  imgui.Unindent(indent_w or 0)     
end

M.Indent = function(indent_w)
   imgui.Indent(indent_w or 0)                  
end

M.LeftArrowButton = function()
  return imgui.LeftArrowButton()
end

M.RightArrowButton = function()
  return imgui.RightArrowButton()
end

M.UpArrowButton = function()
  return imgui.UpArrowButton()
end


M.DownArrowButton = function()
  return imgui.DownArrowButton()
end

M.Checkbox = function(label, checked)
  return imgui.Checkbox(label, checked)
end

M.RadioButton = function(label, selectedId, id)
  if M.radioButtonGroupOpen then
    id = id or M.radioButtonGroupId
    M.radioButtonGroupId = M.radioButtonGroupId + 1
    
    M.radioButtonGroupSid = imgui.RadioButton(label, M.radioButtonGroupSid, id)
    return M.radioButtonGroupSid
  else
    return imgui.radioButton(label, selectedId, id)
  end
end

M.CollapsingHeader = function(label, flags)
  return imgui.CollapsingHeader(label, flags or 0)
end

M.SameLine = function(offsetX, spacing)
  imgui.SameLine(offsetX or 0.0, spacing or -1.0)
end

M.Combo = function(label, currentItem, items)
  return imgui.Combo(label, currentItem, items)
end

M.ListBox = function(label, currentItem, items, heightInItems)
  return imgui.ListBox(label, currentItem, items, heightInItems or 4)
end

M.VSliderInt = function(label, size, value, min, max, flags)
  min = min or INT_SLIDER_MIN
  max = max or INT_SLIDER_MAX
  flags = flags or 0
  return imgui.VSliderInt(label, size, value, min, max, flags)
end

M.VSliderFloat = function(label, size, value, min, max, flags)
  min = min or FLOAT_SLIDER_MIN
  max = max or FLOAT_SLIDER_MAX
  flags = flags or 0
  return imgui.VSliderFloat(label, size, value, min, max, flags)
end

M.SliderInt = function(label, value, min, max, flags)
  min = min or INT_SLIDER_MIN
  max = max or INT_SLIDER_MAX
  flags = flags or 0
  return imgui.SliderInt(label, value, min, max, flags)
end

M.SliderInt2 = function(label, x, y, min, max, flags)
  min = min or INT_SLIDER_MIN
  max = max or INT_SLIDER_MAX
  flags = flags or 0
  return imgui.SliderInt2(label, x, y, min, max, flags)
end

M.SliderInt3 = function(label, x, y, z, min, max, flags)
  min = min or INT_SLIDER_MIN
  max = max or INT_SLIDER_MAX
  flags = flags or 0
  return imgui.SliderInt3(label, x, y, z, min, max, flags)
end

M.SliderInt4 = function(label, x, y, z, w, min, max, flags)
  min = min or INT_SLIDER_MIN
  max = max or INT_SLIDER_MAX
  flags = flags or 0
  return imgui.SliderInt4(label, x, y, z, w, min, max, flags)
end

M.SliderFloat = function(label, value, min, max, flags)
  min = min or FLOAT_SLIDER_MIN
  max = max or FLOAT_SLIDER_MAX
  flags = flags or 0
  return imgui.SliderFloat(label, value, min, max, flags)
end

M.SliderFloat2 = function(label, x, y, min, max, flags)
  min = min or FLOAT_SLIDER_MIN
  max = max or FLOAT_SLIDER_MAX
  flags = flags or 0
  return imgui.SliderFloat2(label, x, y, min, max, flags)
end

M.SliderFloat3 = function(label, x, y, z, min, max, flags)
  min = min or FLOAT_SLIDER_MIN
  max = max or FLOAT_SLIDER_MAX
  flags = flags or 0
  return imgui.SliderFloat3(label, x, y, z, min, max, flags)
end

M.SliderFloat4 = function(label, x, y, z, w, min, max, flags)
  min = min or FLOAT_SLIDER_MIN
  max = max or FLOAT_SLIDER_MAX
  flags = flags or 0
  return imgui.SliderFloat4(label, x, y, z, w, min, max, flags)
end

M.Slider = function(label, value, min, max)
  --Compatibility function, the name "Slider" was used
  --During development, but is now supposed to be SliderFloat
  return M.SliderFloat(label, value, min, max)
end

M.DragFloat4 = function(label, x, y, z, w, speed, min, max, flags)
  min = min or 0
  max = max or 0
  speed = speed or 1
  flags = flags or 0
  return imgui.DragFloat4(label, x, y, z, w, speed, min, max, flags)
end

M.DragFloat3 = function(label, x, y, z, speed, min, max, flags)
  min = min or 0
  max = max or 0
  speed = speed or 1
  flags = flags or 0
  return imgui.DragFloat3(label, x, y, z, speed, min, max, flags)
end

M.DragFloat2 = function(label, x, y, speed, min, max, flags)
  min = min or 0
  max = max or 0
  speed = speed or 1
  flags = flags or 0
  return imgui.DragFloat2(label, x, y, speed, min, max, flags)
end

M.DragInt4 = function(label, x, y, z, w, speed, min, max, flags)
  min = min or 0
  max = max or 0
  speed = speed or 1
  flags = flags or 0
  return imgui.DragInt4(label, x, y, z, w, speed, min, max, flags)
end

M.DragInt3 = function(label, x, y, z, speed, min, max, flags)
  min = min or 0
  max = max or 0
  speed = speed or 1
  flags = flags or 0
  return imgui.DragInt3(label, x, y, z, speed, min, max, flags)
end

M.DragInt2 = function(label, x, y, speed, min, max, flags)
  min = min or 0
  max = max or 0
  speed = speed or 1
  flags = flags or 0
  return imgui.DragInt2(label, x, y, speed, min, max, flags)
end

M.ColorButton = function(dec_id, r, g, b, a, flags, size)
  flags = flags or 0
  size = size or ImVec2(0, 0)
  a = a or 1.0
  return imgui.ColorButton(dec_id, r, g, b, a, flags, size)
end

M.ColorEdit4 = function(label, r, g, b, a, flags)
  flags = flags or 0
  return imgui.ColorEdit4(label, r, g, b, a, flags)
end

M.ColorEdit3 = function(label, r, g, b, flags)
  flags = flags or 0
  return imgui.ColorEdit3(label, r, g, b, flags)
end

M.DragInt = function(label, value, speed, min, max, flags)
  min = min or 0
  max = max or 0
  speed = speed or 1
  flags = flags or 0
  return imgui.DragInt(label, value, speed, min, max, flags)
end

M.DragFloat = function(label, value, speed, min, max, flags)
  min = min or 0
  max = max or 0
  speed = speed or 1
  flags = flags or 0
  return imgui.DragFloat(label, value, speed, min, max, flags)
end

M.Drag = function(label, value, speed, min, max)
  --Compatibility function, the name "Drag" was used
  --During development, but is now supposed to be DragFloat
  return M.DragFloat(label, value, speed, min, max)
end

M.End = function()
  imgui.End()
end

M.Begin = function(name, open, flags)
  open = boolDefault(open, true)
  flags = flags or 0
  return imgui.Begin(name, open, flags)
end

M.InvisibleButton = function(text, size, flags)
  flags = flags or 0
  return imgui.InvisibleButton(text, size, flags) 
end

M.Button = function(text, size)
  return imgui.Button(text, size or ImVec2(0, 0))
end

M.SmallButton = function(text)
  return imgui.SmallButton(text)
end

M.ProgressBar = function(fraction, size, overlay)
  size = size or ImVec2(-1, 0)
  overlay = nullableString(overlay)
  imgui.ProgressBar(fraction, size, overlay)
end

M.Image = function(texture, size, uv0, uv1, tintColor, borderColor)
  uv0 = uv0 or ImVec2(0, 0)
  uv1 = uv1 or ImVec2(1, 1)
  tintColor = tintColor or ImVec4(1, 1, 1, 1)
  borderColor = borderColor or ImVec4(0, 0, 0, 0)
  imgui.Image(texture, size, uv0, uv1, tintColor, borderColor)
end

--Demo window. Draw this for cool examples
M.ShowDemoWindow = function(show) 
  show = boolDefault(show, true)
  return imgui.ShowDemoWindow(show)
end

--Make global constants
ImGuiColorEditFlags_None                    = 0
ImGuiColorEditFlags_NoAlpha                 = 1 << 1
ImGuiColorEditFlags_NoPicker                = 1 << 2
ImGuiColorEditFlags_NoOptions               = 1 << 3
ImGuiColorEditFlags_NoSmallPreview          = 1 << 4
ImGuiColorEditFlags_NoInputs                = 1 << 5
ImGuiColorEditFlags_NoTooltip               = 1 << 6
ImGuiColorEditFlags_NoLabel                 = 1 << 7
ImGuiColorEditFlags_NoSidePreview           = 1 << 8
ImGuiColorEditFlags_NoDragDrop              = 1 << 9
ImGuiColorEditFlags_NoBorder                = 1 << 10
ImGuiColorEditFlags_AlphaBar                = 1 << 16
ImGuiColorEditFlags_AlphaPreview            = 1 << 17
ImGuiColorEditFlags_AlphaPreviewHalf        = 1 << 18
ImGuiColorEditFlags_HDR                     = 1 << 19
ImGuiColorEditFlags_DisplayRGB              = 1 << 20
ImGuiColorEditFlags_DisplayHSV              = 1 << 21
ImGuiColorEditFlags_DisplayHex              = 1 << 22
ImGuiColorEditFlags_Uint8                   = 1 << 23
ImGuiColorEditFlags_Float                   = 1 << 24
ImGuiColorEditFlags_PickerHueBar            = 1 << 25
ImGuiColorEditFlags_PickerHueWheel          = 1 << 26
ImGuiColorEditFlags_InputRGB                = 1 << 27
ImGuiColorEditFlags_InputHSV                = 1 << 28
    
ImGuiStyleVar_Alpha                         = 0
ImGuiStyleVar_WindowPadding                 = 1
ImGuiStyleVar_WindowRounding                = 2
ImGuiStyleVar_WindowBorderSize              = 3
ImGuiStyleVar_WindowMinSize                 = 4
ImGuiStyleVar_WindowTitleAlign              = 5
ImGuiStyleVar_ChildRounding                 = 6
ImGuiStyleVar_ChildBorderSize               = 7
ImGuiStyleVar_PopupRounding                 = 8
ImGuiStyleVar_PopupBorderSize               = 9
ImGuiStyleVar_FramePadding                  = 10
ImGuiStyleVar_FrameRounding                 = 11
ImGuiStyleVar_FrameBorderSize               = 12
ImGuiStyleVar_ItemSpacing                   = 13
ImGuiStyleVar_ItemInnerSpacing              = 14
ImGuiStyleVar_IndentSpacing                 = 15
ImGuiStyleVar_ScrollbarSize                 = 16
ImGuiStyleVar_ScrollbarRounding             = 17
ImGuiStyleVar_GrabMinSize                   = 18
ImGuiStyleVar_GrabRounding                  = 19
ImGuiStyleVar_TabRounding                   = 20
ImGuiStyleVar_ButtonTextAlign               = 21
ImGuiStyleVar_SelectableTextAlign           = 22
ImGuiStyleVar_COUNT                         = 23

ImGuiCol_Text                               = 0
ImGuiCol_TextDisabled                       = 1
ImGuiCol_WindowBg                           = 2
ImGuiCol_ChildBg                            = 3
ImGuiCol_PopupBg                            = 4
ImGuiCol_Border                             = 5
ImGuiCol_BorderShadow                       = 6
ImGuiCol_FrameBg                            = 7
ImGuiCol_FrameBgHovered                     = 8
ImGuiCol_FrameBgActive                      = 9
ImGuiCol_TitleBg                            = 10
ImGuiCol_TitleBgActive                      = 11
ImGuiCol_TitleBgCollapsed                   = 12
ImGuiCol_MenuBarBg                          = 13
ImGuiCol_ScrollbarBg                        = 14
ImGuiCol_ScrollbarGrab                      = 15
ImGuiCol_ScrollbarGrabHovered               = 16
ImGuiCol_ScrollbarGrabActive                = 17
ImGuiCol_CheckMark                          = 18
ImGuiCol_SliderGrab                         = 19
ImGuiCol_SliderGrabActive                   = 20
ImGuiCol_Button                             = 21
ImGuiCol_ButtonHovered                      = 22
ImGuiCol_ButtonActive                       = 23
ImGuiCol_Header                             = 24
ImGuiCol_HeaderHovered                      = 25
ImGuiCol_HeaderActive                       = 26
ImGuiCol_Separator                          = 27
ImGuiCol_SeparatorHovered                   = 28
ImGuiCol_SeparatorActive                    = 29
ImGuiCol_ResizeGrip                         = 30
ImGuiCol_ResizeGripHovered                  = 31
ImGuiCol_ResizeGripActive                   = 32
ImGuiCol_Tab                                = 33
ImGuiCol_TabHovered                         = 34
ImGuiCol_TabActive                          = 35
ImGuiCol_TabUnfocused                       = 36
ImGuiCol_TabUnfocusedActive                 = 37
ImGuiCol_PlotLines                          = 38
ImGuiCol_PlotLinesHovered                   = 39
ImGuiCol_PlotHistogram                      = 40
ImGuiCol_PlotHistogramHovered               = 41
ImGuiCol_TextSelectedBg                     = 42
ImGuiCol_DragDropTarget                     = 43
ImGuiCol_NavHighlight                       = 44
ImGuiCol_NavWindowingHighlight              = 45
ImGuiCol_NavWindowingDimBg                  = 46
ImGuiCol_ModalWindowDimBg                   = 47
ImGuiCol_COUNT                              = 48

ImGuiWindowFlags_None                       = 0
ImGuiWindowFlags_NoTitleBar                 = 1 << 0
ImGuiWindowFlags_NoResize                   = 1 << 1
ImGuiWindowFlags_NoMove                     = 1 << 2
ImGuiWindowFlags_NoScrollbar                = 1 << 3
ImGuiWindowFlags_NoScrollWithMouse          = 1 << 4
ImGuiWindowFlags_NoCollapse                 = 1 << 5
ImGuiWindowFlags_AlwaysAutoResize           = 1 << 6
ImGuiWindowFlags_NoBackground               = 1 << 7
ImGuiWindowFlags_NoSavedSettings            = 1 << 8
ImGuiWindowFlags_NoMouseInputs              = 1 << 9
ImGuiWindowFlags_MenuBar                    = 1 << 10
ImGuiWindowFlags_HorizontalScrollbar        = 1 << 11
ImGuiWindowFlags_NoFocusOnAppearing         = 1 << 12
ImGuiWindowFlags_NoBringToFrontOnFocus      = 1 << 13
ImGuiWindowFlags_AlwaysVerticalScrollbar    = 1 << 14
ImGuiWindowFlags_AlwaysHorizontalScrollbar  = 1<< 15
ImGuiWindowFlags_AlwaysUseWindowPadding     = 1 << 16
ImGuiWindowFlags_NoNavInputs                = 1 << 18
ImGuiWindowFlags_NoNavFocus                 = 1 << 19
ImGuiWindowFlags_UnsavedDocument            = 1 << 20
ImGuiWindowFlags_NoDocking                  = 1 << 21
ImGuiWindowFlags_NoNav                      = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus
ImGuiWindowFlags_NoDecoration               = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
ImGuiWindowFlags_NoInputs                   = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus

ImGuiCond_None          = 0   
ImGuiCond_Always        = 1 << 0
ImGuiCond_Once          = 1 << 1
ImGuiCond_FirstUseEver  = 1 << 2
ImGuiCond_Appearing     = 1 << 3

ImGuiInputTextFlags_None                = 0
ImGuiInputTextFlags_CharsDecimal        = 1 << 0
ImGuiInputTextFlags_CharsHexadecimal    = 1 << 1
ImGuiInputTextFlags_CharsUppercase      = 1 << 2
ImGuiInputTextFlags_CharsNoBlank        = 1 << 3
ImGuiInputTextFlags_AutoSelectAll       = 1 << 4
ImGuiInputTextFlags_EnterReturnsTrue    = 1 << 5
ImGuiInputTextFlags_CallbackCompletion  = 1 << 6
ImGuiInputTextFlags_CallbackHistory     = 1 << 7
ImGuiInputTextFlags_CallbackAlways      = 1 << 8
ImGuiInputTextFlags_CallbackCharFilter  = 1 << 9
ImGuiInputTextFlags_AllowTabInput       = 1 << 10
ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 11
ImGuiInputTextFlags_NoHorizontalScroll  = 1 << 12
ImGuiInputTextFlags_AlwaysInsertMode    = 1 << 13
ImGuiInputTextFlags_ReadOnly            = 1 << 14
ImGuiInputTextFlags_Password            = 1 << 15
ImGuiInputTextFlags_NoUndoRedo          = 1 << 16
ImGuiInputTextFlags_CharsScientific     = 1 << 17
ImGuiInputTextFlags_CallbackResize      = 1 << 18
ImGuiInputTextFlags_CallbackEdit        = 1 << 19

ImGuiSliderFlags_None                   = 0
ImGuiSliderFlags_ClampOnInput           = 1 << 4 
ImGuiSliderFlags_Logarithmic            = 1 << 5 
ImGuiSliderFlags_NoRoundToFormat        = 1 << 6 
ImGuiSliderFlags_NoInput                = 1 << 7 

ImGuiButtonFlags_None                   = 0
ImGuiButtonFlags_MouseButtonLeft        = 1 << 0
ImGuiButtonFlags_MouseButtonRight       = 1 << 1
ImGuiButtonFlags_MouseButtonMiddle      = 1 << 2

ImGuiTabBarFlags_None                           = 0
ImGuiTabBarFlags_Reorderable                    = 1 << 0
ImGuiTabBarFlags_AutoSelectNewTabs              = 1 << 1
ImGuiTabBarFlags_TabListPopupButton             = 1 << 2
ImGuiTabBarFlags_NoCloseWithMiddleMouseButton   = 1 << 3
ImGuiTabBarFlags_NoTabListScrollingButtons      = 1 << 4
ImGuiTabBarFlags_NoTooltip                      = 1 << 5
ImGuiTabBarFlags_FittingPolicyResizeDown        = 1 << 6
ImGuiTabBarFlags_FittingPolicyScroll            = 1 << 7

ImGuiTabItemFlags_None                          = 0
ImGuiTabItemFlags_UnsavedDocument               = 1 << 0
ImGuiTabItemFlags_SetSelected                   = 1 << 1
ImGuiTabItemFlags_NoCloseWithMiddleMouseButton  = 1 << 2
ImGuiTabItemFlags_NoPushId                      = 1 << 3
ImGuiTabItemFlags_NoTooltip                     = 1 << 4

ImGuiMouseButton_Left = 0
ImGuiMouseButton_Right = 1
ImGuiMouseButton_Middle = 2

ImGuiHoveredFlags_None                          = 0
ImGuiHoveredFlags_ChildWindows                  = 1 << 0
ImGuiHoveredFlags_RootWindow                    = 1 << 1
ImGuiHoveredFlags_AnyWindow                     = 1 << 2
ImGuiHoveredFlags_AllowWhenBlockedByPopup       = 1 << 3
--ImGuiHoveredFlags_AllowWhenBlockedByModal     = 1 << 4   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
ImGuiHoveredFlags_AllowWhenBlockedByActiveItem  = 1 << 5
ImGuiHoveredFlags_AllowWhenOverlapped           = 1 << 6
ImGuiHoveredFlags_AllowWhenDisabled             = 1 << 7
ImGuiHoveredFlags_RectOnly                      = ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenOverlapped
ImGuiHoveredFlags_RootAndChildWindows           = ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows
    
ImGuiFocusedFlags_None                          = 0
ImGuiFocusedFlags_ChildWindows                  = 1 << 0
ImGuiFocusedFlags_RootWindow                    = 1 << 1
ImGuiFocusedFlags_AnyWindow                     = 1 << 2
ImGuiFocusedFlags_RootAndChildWindows           = ImGuiFocusedFlags_RootWindow | ImGuiFocusedFlags_ChildWindows
    
return M