import AbstractView from "../AbstractView.js";
import { formFromOptions } from "../form/Form.js";

export default class MQTT extends AbstractView {
    constructor(params) {
        super(params);
    }

    async getHtml() {
        const response = await fetch("/api/mqtt");
        const js = await response.json();       

        const opts = [
            {
                name: "hostname", 
                info: {type: "text", label: "Hostname", value: js.hostname}
            },
            {
                name: "port", 
                info: {type: "number", label: "Port", value: js.port}
            },
            {
                name: "user", 
                info: {type: "text", label: "User", value: js.user}
            },
            {
                name: "pass", 
                info: {type: "password", label: "Password", value: js.pass}
            },
            {
                name: "sensorTopics",
                info: {type: "texts", label: "Sensor topics", value: js.sensorTopics ? js.sensorTopics : []}
            }
        ];

        return `
            <h1>MQTT config</h1>
            ${formFromOptions("/api/mqtt", opts, "Submit")}
        `;
    }
}
