import AbstractView from "../AbstractView.js";
import { getSensorDetails } from "./SensorHelpers.js";

export default class extends AbstractView {
    constructor(params) {
        super(params);
        this.setTitle("Sensor: ");
    }

    async getHtml() {
        const details = await getSensorDetails(this.params.uuid);
        const keyValueDetails = Object.values(details).filter(item => item.type == "key/value");

        return `
            <h1>Sensor ${this.params.uuid}</h1>

            <table>
            ${keyValueDetails
                .map(item => `
                <tr>
                    <th>${item.label}</th>
                    <th>${item.value}</th>
                </tr>
                `)
                .join("\n")
            }
            </table>
        `;
    }
}
