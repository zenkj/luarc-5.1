local function foo ()
  local upval = {}
  function gfoo() 
    upval.a = 5
  end
  --return nil -- comment this line, then upval is released
end

foo()

gfoo()

