----------------------------------------------------------
-- KeyWriter.lua
----------------------------------------------------------


----------------------------------------------------------
-- Open log file first
----------------------------------------------------------
local file = io.open("framedata.csv", "w") -- TODO: modify filename if needed
if file == nil then
    error("File could not be opened.")
end


----------------------------------------------------------
local key = joypad.get(0)
local s="frame"
for k,v in pairs(key) do
	s=s .. "," .. k
end
s=s .. "\n"
file:write(s)

--------------
-- main loop
--------------
while emu.frameadvance do
	emu.frameadvance()
	
	local s=""
	
	s=s .. emu.framecount() 
	
	local key = joypad.get(0)
	for k,v in pairs(key) do
		if type(v) == "boolean" then
			if v then
				s=s .. "," .. 1 
			else
				s=s .. "," .. 0  
			end
		else
			s=s .. "," .. v 
		end
	end
	
	s=s .. "\n"
	file:write(s)
	
end

io.close(file)

