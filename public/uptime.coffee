# Uptime display functionality
updateUptimeDisplay = ->
  $.ajax
    url: '/uptime'
    method: 'GET'
    success: (data) ->
      if data && data.uptime_seconds != undefined
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
        $('#uptime-value').text('Offline')
    error: ->
      $('#uptime-value').text('Offline')

# Update uptime display every 5 seconds
setInterval(updateUptimeDisplay, 5000)

# Initial update
$(document).ready ->
  updateUptimeDisplay()
