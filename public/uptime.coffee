# Uptime display functionality
serverConnected = false

updateUptimeDisplay = ->
  $.ajax
    url: '/uptime'
    method: 'GET'
    timeout: 2000  # Add timeout to detect disconnection faster
    success: (data) ->
      if data.status == 'offline'
        serverConnected = false
        $('#uptime-value').text('Offline')
      else if data && data.uptime_seconds != undefined
        serverConnected = true
        seconds = data.uptime_seconds
        days = Math.floor(seconds / 86400)
        hours = Math.floor((seconds % 86400) / 3600)
        minutes = Math.floor((seconds % 3600) / 60)
        seconds = seconds % 60
        
        uptimeText = ""
        if days > 0
          uptimeText += "#{days}d "
        if hours > 0 || days > 0
          uptimeText += "#{hours}h "
        if minutes > 0 || hours > 0 || days > 0
          uptimeText += "#{minutes}m "
        uptimeText += "#{seconds}s"
        
        $('#uptime-value').text(uptimeText)
      else
        serverConnected = false
        $('#uptime-value').text('Offline')
    error: ->
      serverConnected = false
      $('#uptime-value').text('Offline')

# Update uptime display every 5 seconds
setInterval(updateUptimeDisplay, 5000)

# Initial update
$(document).ready ->
  updateUptimeDisplay()
