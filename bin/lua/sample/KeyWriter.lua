----------------------------------------------------------
-- KeyWriter.lua
----------------------------------------------------------


----------------------------------------------------------
-- Open log file first
----------------------------------------------------------
local file = io.open("framedata.txt", "w") -- TODO: modify filename if needed
if file == nil then
    error("File could not be opened.")
end

----------------------------------------------------------
while emu.frameadvance do
	local s=""
	
	s=s .. emu.framecount() .. " "
	
	local key = joypad.get(0)
	for k,v in pairs(key) do
		if type(v) == "boolean" then
			if v then
				s=s .. k .. "=" .. 1 .. " "
			else
				s=s .. k .. "=" .. 0 .. " "
			end
		else
			s=s .. k .. "=" .. v .. " "
		end
	end
	
	s=s .. "\n"
	file:write(s)
	emu.frameadvance()
end

io.close(file)

