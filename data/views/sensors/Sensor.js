import AbstractView from "../AbstractView.js";
import { getSensorInfo } from "./SensorHelpers.js";

export default class extends AbstractView {
    constructor(params) {
        super(params);
        this.setTitle("Sensor: ");
    }

    async getHtml() {
        const {options, config} = getSensorInfo(this.params.uuid);

        return `
            <h1>Sensor ${this.params.id}</h1>
        `;
    }
}
