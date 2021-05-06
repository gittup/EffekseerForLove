local effekseer = require('effekseer')
local manager
local effect
local img

function love.load(args)
	love.window.setMode(1280, 800)

	manager = effekseer.newEffectManager()
	effect = effekseer.newEffect(manager, "Pierre01/LightningStrike.efk")
	img = love.graphics.newImage('Pierre01/Texture/Burst_1.png')
end

function love.mousepressed(x, y, button)
	local h = manager:play(effect, x, y, 0)
end

local tot_dt = 0
function love.update(dt)
	tot_dt = tot_dt + dt
	if tot_dt >= 1 then
		tot_dt = tot_dt - 1
		print("FPS: " .. love.timer.getFPS())
	end
	manager:update(dt)
end

function love.draw()
	-- Draw a basic image to see how the effekseer images overlap
	love.graphics.setColor(1, 0, 0, 1)
	love.graphics.draw(img, 400, 200)

	-- Draw all effects using the manager
	manager:draw()

	-- Draw another image to show in front of the effects
	love.graphics.draw(img, 300, 300)
end
