import React, { useState, useEffect } from 'react';
import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend,
} from 'chart.js';
import { Line } from 'react-chartjs-2';

// Register Chart.js components
ChartJS.register(
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend
);

// Main App component
const App = () => {
    // Hardcoded raw data from your trace files
    const rawData = `Policy	Associativity	CacheSize	BlockSize	Hits	Misses	HitRate	TraceFile
lru	1	1024	64	230993	72200	76.19	swim.trace
lru	1	2048	64	248285	54908	81.89	swim.trace
lru	1	4096	64	285247	17946	94.08	swim.trace
lru	1	8192	64	292767	10426	96.56	swim.trace
lru	1	16384	64	296867	6326	97.91	swim.trace
lru	4	1024	64	259472	43721	85.58	swim.trace
lru	4	2048	64	292115	11078	96.35	swim.trace
lru	4	4096	64	296172	7021	97.68	swim.trace
lru	4	8192	64	298661	4532	98.51	swim.trace
lru	4	16384	64	299449	3744	98.77	swim.trace
lru	4	16384	16	291463	11730	96.13	swim.trace
lru	4	16384	32	296527	6666	97.8	swim.trace
lru	4	16384	64	299449	3744	98.77	swim.trace
lru	4	16384	128	301053	2140	99.29	swim.trace
lru	1	16384	64	296867	6326	97.91	swim.trace
lru	2	16384	64	299111	4082	98.65	swim.trace
lru	4	16384	64	299449	3744	98.77	swim.trace
lru	8	16384	64	299484	3709	98.78	swim.trace
lru	256	16384	64	299549	3644	98.8	swim.trace
fifo	1	1024	64	230993	72200	76.19	swim.trace
fifo	1	2048	64	248285	54908	81.89	swim.trace
fifo	1	4096	64	285247	17946	94.08	swim.trace
fifo	1	8192	64	292767	10426	96.56	swim.trace
fifo	1	16384	64	296867	6326	97.91	swim.trace
fifo	4	1024	64	254296	48897	83.87	swim.trace
fifo	4	2048	64	288988	14205	95.31	swim.trace
fifo	4	4096	64	294358	8835	97.09	swim.trace
fifo	4	8192	64	297303	5890	98.06	swim.trace
fifo	4	16384	64	298822	4371	98.56	swim.trace
fifo	4	16384	16	290144	13049	95.7	swim.trace
fifo	4	16384	32	295647	7546	97.51	swim.trace
fifo	4	16384	64	298822	4371	98.56	swim.trace
fifo	4	16384	128	300584	2609	99.14	swim.trace
fifo	1	16384	64	296867	6326	97.91	swim.trace
fifo	2	16384	64	298604	4589	98.49	swim.trace
fifo	4	16384	64	298822	4371	98.56	swim.trace
fifo	8	16384	64	298839	4354	98.56	swim.trace
fifo	256	16384	64	298884	4309	98.58	swim.trace
lru	1	1024	64	428856	86827	83.16	gcc.trace
lru	1	2048	64	458735	56948	88.96	gcc.trace
lru	1	4096	64	488882	26801	94.8	gcc.trace
lru	1	8192	64	496982	18701	96.37	gcc.trace
lru	1	16384	64	506361	9322	98.19	gcc.trace
lru	4	1024	64	474784	40899	92.07	gcc.trace
lru	4	2048	64	491279	24404	95.27	gcc.trace
lru	4	4096	64	501926	13757	97.33	gcc.trace
lru	4	8192	64	509502	6181	98.8	gcc.trace
lru	4	16384	64	510978	4705	99.09	gcc.trace
lru	4	16384	16	503048	12635	97.55	gcc.trace
lru	4	16384	32	508326	7357	98.57	gcc.trace
lru	4	16384	64	510978	4705	99.09	gcc.trace
lru	4	16384	128	512006	3677	99.29	gcc.trace
lru	1	16384	64	506361	9322	98.19	gcc.trace
lru	2	16384	64	510125	5558	98.92	gcc.trace
lru	4	16384	64	510978	4705	99.09	gcc.trace
lru	8	16384	64	511178	4505	99.13	gcc.trace
lru	256	16384	64	511250	4433	99.14	gcc.trace
fifo	1	1024	64	428856	86827	83.16	gcc.trace
fifo	1	2048	64	458735	56948	88.96	gcc.trace
fifo	1	4096	64	488882	26801	94.8	gcc.trace
fifo	1	8192	64	496982	18701	96.37	gcc.trace
fifo	1	16384	64	506361	9322	98.19	gcc.trace
fifo	4	1024	64	469181	46502	90.98	gcc.trace
fifo	4	2048	64	487345	28338	94.5	gcc.trace
fifo	4	4096	64	498506	17177	96.67	gcc.trace
fifo	4	8192	64	508253	7430	98.56	gcc.trace
fifo	4	16384	64	510286	5397	98.95	gcc.trace
fifo	4	16384	16	502218	13465	97.39	gcc.trace
fifo	4	16384	32	507612	8071	98.43	gcc.trace
fifo	4	16384	64	510286	5397	98.95	gcc.trace
fifo	4	16384	128	511170	4513	99.12	gcc.trace
fifo	1	16384	64	506361	9322	98.19	gcc.trace
fifo	2	16384	64	298604	4589	98.49	swim.trace
fifo	4	16384	64	510286	5397	98.95	gcc.trace
fifo	8	16384	64	510472	5211	98.99	gcc.trace
fifo	256	16384	64	510570	5113	99.01	gcc.trace
lru	4	1024	64	2	3	40	read01.trace
fifo	4	2048	64	9	1	90	read02.trace
lru	8	4096	128	8	1	88.89	read03.trace
fifo	2	8192	32	0	7	0	read04.trace
lru	4	16384	64	2	5	28.57	read05.trace
fifo	1	1024	16	0	7	0	read06.trace
lru	8	2048	64	3	5	37.5	read08.trace
fifo	4	4096	64	2	3	40	write01.trace`;

    const [data, setData] = useState([]);

    // Helper function to parse the raw data string
    const parseData = (data) => {
        const lines = data.trim().split('\n');
        const headers = lines[0].split('\t').map(h => h.trim());
        const parsed = lines.slice(1).map(line => {
            const values = line.split('\t').map(v => v.trim());
            const obj = {};
            headers.forEach((header, i) => {
                if (['Associativity', 'CacheSize', 'BlockSize', 'Hits', 'Misses', 'HitRate'].includes(header)) {
                    obj[header] = parseFloat(values[i]);
                } else {
                    obj[header] = values[i];
                }
            });
            return obj;
        });
        return parsed;
    };

    // Parse data on component mount
    useEffect(() => {
        setData(parseData(rawData));
    }, []);

    // Filter and prepare data for the four charts
    const plot1Data = () => {
        const datasets = {};
        data.forEach(d => {
            // Filter for relevant data points
            if (d.BlockSize === 64 && !d.TraceFile.includes('read') && !d.TraceFile.includes('write')) {
                const key = `${d.Policy}_${d.Associativity}`;
                if (!datasets[key]) {
                    datasets[key] = {
                        label: '',
                        data: [],
                        borderColor: '',
                        backgroundColor: 'rgba(0,0,0,0)',
                        tension: 0.1,
                    };
                }
                // Check for specific associativities to match the graph's intent
                if (d.Policy === 'lru' && (d.Associativity === 1 || d.Associativity === 4 || d.Associativity === 256)) {
                    datasets[key].label = `LRU - ${d.Associativity === 1 ? 'Direct Mapped' : d.Associativity + '-way'}`;
                    datasets[key].data.push({ x: d.CacheSize, y: d.HitRate });
                }
                if (d.Policy === 'fifo' && (d.Associativity === 1 || d.Associativity === 4 || d.Associativity === 256)) {
                    datasets[key].label = `FIFO - ${d.Associativity === 1 ? 'Direct Mapped' : d.Associativity + '-way'}`;
                    datasets[key].data.push({ x: d.CacheSize, y: d.HitRate });
                }
            }
        });

        // Sort data points and assign colors
        const sortedDatasets = Object.values(datasets).map(dataset => {
            dataset.data.sort((a, b) => a.x - b.x);
            return dataset;
        });

        // Assign colors and dash patterns for clarity
        const colors = ['#8884d8', '#82ca9d', '#ffc658', '#4e79a7', '#e15759', '#f28e2b'];
        sortedDatasets[0].borderColor = colors[0];
        sortedDatasets[1].borderColor = colors[1];
        sortedDatasets[2].borderColor = colors[2];
        sortedDatasets[3].borderColor = colors[3];
        sortedDatasets[3].borderDash = [5, 5];
        sortedDatasets[4].borderColor = colors[4];
        sortedDatasets[4].borderDash = [5, 5];
        sortedDatasets[5].borderColor = colors[5];
        sortedDatasets[5].borderDash = [5, 5];

        return { datasets: sortedDatasets };
    };

    const plot2Data = () => {
        const lruData = [];
        const fifoData = [];
        data.forEach(d => {
            if (d.Associativity === 4 && d.BlockSize === 64 && !d.TraceFile.includes('read') && !d.TraceFile.includes('write')) {
                if (d.Policy === 'lru') {
                    lruData.push({ x: d.CacheSize, y: d.HitRate });
                } else if (d.Policy === 'fifo') {
                    fifoData.push({ x: d.CacheSize, y: d.HitRate });
                }
            }
        });
        lruData.sort((a, b) => a.x - b.x);
        fifoData.sort((a, b) => a.x - b.x);
        return {
            datasets: [
                { label: 'LRU', data: lruData, borderColor: '#8884d8', backgroundColor: 'rgba(0,0,0,0)', tension: 0.1 },
                { label: 'FIFO', data: fifoData, borderColor: '#82ca9d', backgroundColor: 'rgba(0,0,0,0)', tension: 0.1 }
            ]
        };
    };

    const plot3Data = () => {
        const lruData = [];
        const fifoData = [];
        data.forEach(d => {
            if (d.CacheSize === 16384 && d.Associativity === 4 && !d.TraceFile.includes('read') && !d.TraceFile.includes('write')) {
                if (d.Policy === 'lru') {
                    lruData.push({ x: d.BlockSize, y: d.HitRate });
                } else if (d.Policy === 'fifo') {
                    fifoData.push({ x: d.BlockSize, y: d.HitRate });
                }
            }
        });
        lruData.sort((a, b) => a.x - b.x);
        fifoData.sort((a, b) => a.x - b.x);
        return {
            datasets: [
                { label: 'LRU', data: lruData, borderColor: '#8884d8', backgroundColor: 'rgba(0,0,0,0)', tension: 0.1 },
                { label: 'FIFO', data: fifoData, borderColor: '#82ca9d', backgroundColor: 'rgba(0,0,0,0)', tension: 0.1 }
            ]
        };
    };

    const plot4Data = () => {
        const lruData = [];
        const fifoData = [];
        data.forEach(d => {
            if (d.CacheSize === 16384 && d.BlockSize === 64 && !d.TraceFile.includes('read') && !d.TraceFile.includes('write')) {
                if (d.Policy === 'lru') {
                    lruData.push({ x: d.Associativity, y: d.HitRate });
                } else if (d.Policy === 'fifo') {
                    fifoData.push({ x: d.Associativity, y: d.HitRate });
                }
            }
        });
        lruData.sort((a, b) => a.x - b.x);
        fifoData.sort((a, b) => a.x - b.x);
        return {
            datasets: [
                { label: 'LRU', data: lruData, borderColor: '#8884d8', backgroundColor: 'rgba(0,0,0,0)', tension: 0.1 },
                { label: 'FIFO', data: fifoData, borderColor: '#82ca9d', backgroundColor: 'rgba(0,0,0,0)', tension: 0.1 }
            ]
        };
    };

    // Common chart options
    const commonOptions = {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                position: 'top',
            },
            tooltip: {
                callbacks: {
                    label: function (context) {
                        let label = context.dataset.label || '';
                        if (label) {
                            label += ': ';
                        }
                        if (context.parsed.y !== null) {
                            label += context.parsed.y.toFixed(2) + '%';
                        }
                        return label;
                    }
                }
            },
        },
        scales: {
            x: {
                type: 'linear',
                title: {
                    display: true,
                    text: ''
                },
                ticks: {
                    callback: (value) => value.toString(),
                },
            },
            y: {
                title: {
                    display: true,
                    text: 'Hit Rate (%)'
                },
                min: 70,
                max: 100,
            }
        }
    };

    // Check if data is loaded before rendering charts
    if (data.length === 0) {
        return <div className="text-center p-8 text-xl font-semibold">Loading charts...</div>;
    }

    return (
        <div className="p-8 font-['Inter'] bg-gray-100 min-h-screen">
            <div className="container mx-auto">
                <div className="text-center mb-10">
                    <h1 className="text-4xl font-extrabold text-blue-600 mb-2">Cache Simulation Results</h1>
                    <p className="text-lg text-gray-600">Analysis of cache performance based on varying parameters.</p>
                </div>

                {/* Grid for the charts */}
                <div className="grid grid-cols-1 lg:grid-cols-2 gap-10">

                    {/* Chart 1: Hit Rate vs. Cache Size for Associativities */}
                    <div className="bg-white rounded-xl shadow-lg p-6">
                        <h2 className="text-2xl font-bold text-center mb-6">Plot 1: Hit Rate vs. Cache Size (Block Size: 64B)</h2>
                        <div className="h-[400px]">
                            <Line data={plot1Data()} options={{
                                ...commonOptions,
                                scales: {
                                    ...commonOptions.scales,
                                    x: {
                                        ...commonOptions.scales.x,
                                        title: { ...commonOptions.scales.x.title, text: 'Cache Size (KB)' },
                                        ticks: {
                                            callback: (value) => `${value / 1024}`, // Convert bytes to KB
                                        },
                                    },
                                    y: { ...commonOptions.scales.y, min: 70, max: 100 }
                                }
                            }} />
                        </div>
                    </div>

                    {/* Chart 2: Hit Rate vs. Cache Size for Replacement Policies */}
                    <div className="bg-white rounded-xl shadow-lg p-6">
                        <h2 className="text-2xl font-bold text-center mb-6">Plot 2: Hit Rate vs. Cache Size (Associativity: 4-way, Block Size: 64B)</h2>
                        <div className="h-[400px]">
                            <Line data={plot2Data()} options={{
                                ...commonOptions,
                                scales: {
                                    ...commonOptions.scales,
                                    x: {
                                        ...commonOptions.scales.x,
                                        title: { ...commonOptions.scales.x.title, text: 'Cache Size (KB)' },
                                        ticks: {
                                            callback: (value) => `${value / 1024}`, // Convert bytes to KB
                                        },
                                    },
                                    y: { ...commonOptions.scales.y, min: 70, max: 100 }
                                }
                            }} />
                        </div>
                    </div>

                    {/* Chart 3: Hit Rate vs. Block Size */}
                    <div className="bg-white rounded-xl shadow-lg p-6">
                        <h2 className="text-2xl font-bold text-center mb-6">Plot 3: Hit Rate vs. Block Size (Cache Size: 16KB, Associativity: 4-way)</h2>
                        <div className="h-[400px]">
                            <Line data={plot3Data()} options={{
                                ...commonOptions,
                                scales: {
                                    ...commonOptions.scales,
                                    x: {
                                        ...commonOptions.scales.x,
                                        title: { ...commonOptions.scales.x.title, text: 'Block Size (Bytes)' },
                                    },
                                    y: { ...commonOptions.scales.y, min: 95, max: 100 }
                                }
                            }} />
                        </div>
                    </div>

                    {/* Chart 4: Hit Rate vs. Associativity */}
                    <div className="bg-white rounded-xl shadow-lg p-6">
                        <h2 className="text-2xl font-bold text-center mb-6">Plot 4: Hit Rate vs. Associativity (Cache Size: 16KB, Block Size: 64B)</h2>
                        <div className="h-[400px]">
                            <Line data={plot4Data()} options={{
                                ...commonOptions,
                                scales: {
                                    ...commonOptions.scales,
                                    x: {
                                        ...commonOptions.scales.x,
                                        title: { ...commonOptions.scales.x.title, text: 'Associativity' },
                                    },
                                    y: { ...commonOptions.scales.y, min: 97, max: 100 }
                                }
                            }} />
                        </div>
                    </div>
                </div>
            </div>
        </div>
    );
};

export default App;
