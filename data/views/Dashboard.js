// frontend/static/js/views/Dashboard.js
import AbstractView from "./AbstractView.js";

export default class extends AbstractView {
    constructor(params) {
        super(params);
        this.setTitle("Dashboard");
    }

    async getHtml() {
        return `
            <h1>Welcome to OpenAir!</h1>
            OpenAir works by reading sensor data from the configured sensors and then using that data to calculate a desired fan speed. 
            This calculation is performed by - you guessed it - calculators that are configured to read the sensors. All calculators report their desired speeds 
            to the speedcontroller, which sets the fan to the highest calculated speed.

            This interface allows you to configurate your OpenAir ventillation box.<br>
            Configuration consists of the following steps:
            <ol>
                <li>Configure <a href="/mqtt" data-link="">MQTT</a> (optional) if you want to be able to use mqtt based sensors</li>
                <li>Configure the <a href="/sensors" data-link="">sensors</a> that are connected to your ventillation box</li>
                <li>Configure <a href="/calculators" data-link="">calculators</a> for any sensors that should govern your fanspeed</li>
                <li>Configure the <a href="/controller" data-link="">speedcontroller</a></li>
                <li>Configure <a href="/hass" data-link="">Home Assistant integration</a> if needed</li>
                <li>
            </ol>
            </div>
        `;
    }
}
